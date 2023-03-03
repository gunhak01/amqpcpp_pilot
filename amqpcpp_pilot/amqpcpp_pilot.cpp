#define NOMINMAX
#include <amqpcpp.h>
#include <uv.h>

class MyConnectionHandler : public AMQP::ConnectionHandler
{
private:
  uv_loop_t* loop;
  uv_tcp_t* socket;
  uv_write_t write_req;
  uv_buf_t buffer;

public:
  MyConnectionHandler()
  {
    loop = uv_default_loop();
    socket = new uv_tcp_t();
    uv_tcp_init(loop, socket);
  }

  virtual void onData(AMQP::Connection* connection, const char* data, size_t size)
  {
    buffer = uv_buf_init(const_cast<char*>(data), size);
    uv_write(&write_req, reinterpret_cast<uv_stream_t*>(socket), &buffer, 1,
             [](uv_write_t* req, int status) {
      // Check the status and handle any errors
      // ...
    });
  }

  void onReady(AMQP::Connection* connection) override
  {
    std::cout << "Connected to RabbitMQ!" << std::endl;

    // create a channel
    AMQP::Channel channel(connection);

    // declare an exchange
    channel.declareExchange("my-exchange", AMQP::fanout);

    // declare a queue
    channel.declareQueue("my-queue");

    // bind the queue to the exchange
    channel.bindQueue("my-exchange", "my-queue", "my-routing-key");

    // publish a message
    std::string message = "Hello, RabbitMQ!";

    channel.publish("my-exchange", "my-routing-key", message);
  }

  void onError(AMQP::Connection* connection, const char* message) override
  {
    std::cerr << "AMQP error: " << message << std::endl;
  }

  virtual void onClosed(AMQP::Connection* connection)
  {

  }
};

int main()
{
  MyConnectionHandler handler;

  // create a AMQP connection object
  //AMQP::Connection connection(&handler, AMQP::Address("amqp://admin:nimda@10.10.50.242/"));

  AMQP::Address address("amqp://admin:nimda@10.10.50.242:5672/");
  AMQP::Connection connection(&handler, address);
  //AMQP::Login login("admin", "nimda");
  //AMQP::Connection connection(&handler, login, "10.10.50.242");

  handler.onReady(&connection);

  return 0;
}