#include <iostream>
#include <boost/asio.hpp>
using namespace boost::asio::ip;


class Session : public std::enable_shared_from_this<Session>{
public:
    Session(tcp::socket socket):socket_(std::move(socket)){
        std::cout<<"Session created\n";
    }    
    ~Session(){
        std::cout<<"Session closed\n";
    }

    void start(){
        do_read();
    }

private:
    void do_read(){
        auto self(shared_from_this());
        std::cout<<"wait data\n";

        socket_.async_read_some(
            boost::asio::buffer(data_,max_length),
            [this,self](boost::system::error_code ec, size_t length){
                std::cout<<"Read invoked\n";

                if(!ec){
                    std::cout<< "Recived " << length << " bytes: " << std::string(data_,length);
                    do_write(length);
                } else if (ec == boost::asio::error::eof) {
                    std::cout << "Client disconnected gracefully\n";
                } else {
                    std::cerr << "Read error: " << ec.message() << "\n";
                }
            });
    }

    void do_write(size_t length){
        auto self(shared_from_this());
        std::cout << "Writing " << length << " bytes back...\n";
        boost::asio::async_write(socket_,
        boost::asio::buffer(data_,length),
        [this,self](boost::system::error_code ec, size_t bytes_written){
            std::cout<< "Write callback invoked\n";
            if(!ec){
                std::cout << "Wrote " << bytes_written << " bytes\n";
                do_read();
            }else{
                std::cerr << "Write error: " << ec.message() << "\n";
            }
        });
        std::cout << "Async_write scheduled, returning...\n";
    }

    tcp::socket socket_;
    enum{max_length = 1024};
    char data_[max_length];

};

class Server{
public:
    Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context,tcp::endpoint(tcp::v4(),port)){
        std::cout<<"Serv was created on port: " << port << std::endl;
        do_accept();
    }
private:
    void do_accept(){
        std::cout<<"Wait for connection\n";
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket){
            std::cout << "Accept callback invoked\n";
            if(!ec){
                std::make_shared<Session>(std::move(socket))->start();
            } else{
                std::cerr << "Accept error: " << ec.message() << "\n";
            }
        
        });
    }
    boost::asio::ip::tcp::acceptor acceptor_;
};


int main(){
    try{
        std::cout.setf(std::ios::unitbuf);
        boost::asio::io_context io_context;
        Server server(io_context, 8080);
        std::cout<<"Serv was started\n";
        io_context.run();
        std::cout<<"Serv was closed\n";


    }catch(std::exception& e){
        std::cerr<<"Error: " << e.what() <<std::endl;
    }


}