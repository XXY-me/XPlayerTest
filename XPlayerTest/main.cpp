#include "XPlayerTest.h"
#include <QtWidgets/QApplication>


#if defined(Q_OS_MAC)       // Mac
// TODO
#elif defined(Q_OS_UNIX)    // Linux
#pragma comment(lib, "libvlc.a")
#elif defined(Q_OS_WIN)     // Windows
#pragma comment(lib, "libvlc.lib")
#endif

libvlc_instance_t* g_pVlcInstance = nullptr;

int main(int argc, char *argv[])
{
	//≥ı ºªØVlc
	if (nullptr == g_pVlcInstance)
	{
		g_pVlcInstance = libvlc_new(0, nullptr);
		if (nullptr == g_pVlcInstance)
		{
			assert(false);
			qCritical() << "Init Vlc Failed";
			return false;
		}
	}

    QApplication a(argc, argv);
    XPlayerTest w;
    w.show();
	auto ret = a.exec();
	libvlc_release(g_pVlcInstance);
    return ret;
}
