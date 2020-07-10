#include "v2ray.com/core/app/router/config.pb.h"
#include "v2ray.com/core/config.pb.h"
#include "v2ray.com/core/proxy/dokodemo/config.pb.h"
#include "v2ray.com/core/proxy/http/config.pb.h"
#include "v2ray.com/core/proxy/socks/config.pb.h"

#include <QCoreApplication>
#include <QFile>
#include <iostream>

using namespace v2ray;
using namespace v2ray::core;
using namespace v2ray::core::proxy;
using namespace v2ray::core::app;

#define L(x) std::cout << x << std::endl

inline auto httpClientTag = http::ClientConfig::GetDescriptor() -> name();
inline auto httpServerTag = http::ServerConfig::GetDescriptor() -> name();
inline auto socksServerTag = socks::ServerConfig::GetDescriptor() -> name();

template<typename T>
void PrintProxySettings(const std::string &tag, const std::string &buffer);

template<>
void PrintProxySettings<v2ray::core::proxy::http::ServerConfig>(const std::string &tag, const std::string &buffer)
{
    http::ServerConfig s;
    s.ParseFromString(buffer);
    for (const auto &acc : s.accounts())
    {
        L("User: " << acc.first << " Pass: " << acc.second);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    const auto filePath = "/home/leroy/pb.pb";
    //
    QFile f(filePath);
    f.open(QFile::OpenModeFlag::ReadOnly);

    auto content = f.readAll();
    f.close();
    v2ray::core::Config conf;
    conf.ParseFromArray(content.data(), content.size());
    //
    for (const auto &app : conf.app())
    {
        L(app.type());
    }
    for (const auto &ext : conf.extension())
    {
        L(ext.type());
    }
    //
    for (const auto &inbound : conf.inbound())
    {
        auto tag = inbound.tag();
        L(tag);
        if (inbound.has_proxy_settings())
        {
            auto proxySettings = inbound.proxy_settings();
            const auto type = proxySettings.type();
            L(type);
            if (type == httpClientTag)
            {
            }
            if (type == httpServerTag)
            {
                PrintProxySettings<http::ServerConfig>(type, proxySettings.value());
            }
            if (type == socksServerTag)
            {
            }
        }
        if (inbound.has_receiver_settings())
        {
            auto rcvSettings = inbound.receiver_settings();
            L(rcvSettings.type());
        }
    }

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
    return a.exec();
}
