// **********************************************************************
//
// Copyright (c) 2003
// ZeroC, Inc.
// Billerica, MA, USA
//
// All Rights Reserved.
//
// Ice is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License version 2 as published by
// the Free Software Foundation.
//
// **********************************************************************

#include <IceUtil/Thread.h>
#include <Ice/Ice.h>
#include <TestCommon.h>
#include <Test.h>
#include <fstream>

using namespace std;

static int num = 0;

struct N : public C
{
    N()
    {
	++num;
    }

    ~N()
    {
	--num;
    }
};

typedef ::IceInternal::Handle<N> NPtr;

class GarbageProducer : public ::IceUtil::Thread, public ::IceUtil::Monitor< ::IceUtil::Mutex>
{
public:

    GarbageProducer()
    {
	_stop = false;
    }

    virtual void run()
    {
	static ::IceUtil::Time noTime = ::IceUtil::Time::microSeconds(0);

	while(true)
	{
	    {
		::IceUtil::Monitor< ::IceUtil::Mutex>::Lock sync(*this);
		timedWait(noTime);
		if(_stop)
		{
		    return;
		}
	    }

	    {
		NPtr n = new N;
		n->left = n;
	    }

	    {
		NPtr n = new N;
		n->left = n;
		n->right = n;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		n1->left = n2;
		n2->left = n1;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		n1->left = n2;
		n2->left = n1;
		n1->right = n2;
		n2->right = n1;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		n1->left = n2;
		n2->left = n1;
		n1->right = n1;
		n2->right = n2;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		n1->left = n2;
		n2->left = n3;
		n3->left = n1;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		n1->left = n2;
		n2->left = n3;
		n3->left = n1;
		n1->right = n2;
		n2->right = n3;
		n3->right = n1;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		n1->left = n2;
		n2->left = n3;
		n3->left = n1;
		n1->right = n3;
		n2->right = n1;
		n3->right = n2;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		NPtr n4 = new N;
		n1->left = n2;
		n2->left = n1;
		n2->right = n3;
		n3->left = n4;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		NPtr n4 = new N;
		NPtr n5 = new N;
		NPtr n6 = new N;
		n1->left = n2;
		n2->left = n1;
		n2->right = n3;
		n3->left = n4;
		n4->right = n5;
		n5->right = n6;
		n6->right = n5;
	    }
	}
    }

    void stop()
    {
	{
	    ::IceUtil::Monitor< ::IceUtil::Mutex>::Lock sync(*this);
	    _stop = true;
	    notify();
	}
	getThreadControl().join();
    }

    void randomWait()
    {
	::IceUtil::Time waitTime = ::IceUtil::Time::milliSeconds(10 + rand() % 50);
	::IceUtil::Monitor< ::IceUtil::Mutex>::Lock sync(*this);
	timedWait(waitTime);
    }

private:

    bool _stop;
};

class MyApplication : public Ice::Application
{
public:

    virtual int run(int, char* []);
};

int
MyApplication::run(int argc, char* argv[])
{
    cout << "testing single instance... " << flush;
    {
	NPtr n = new N;
	test(num == 1);
	Ice::collectGarbage();
	test(num == 1);
    }
    test(num == 0);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing single instance loop... " << flush;
    {
	NPtr n = new N;
	n->left = n;
	test(num == 1);
	Ice::collectGarbage();
	test(num == 1);
    }
    test(num == 1);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing single instance loop with double pointers... " << flush;
    {
	NPtr n = new N;
	n->left = n;
	n->right = n;
	test(num == 1);
	Ice::collectGarbage();
	test(num == 1);
    }
    test(num == 1);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing double instance loop... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	n1->left = n2;
	n2->left = n1;
	test(num == 2);
	Ice::collectGarbage();
	test(num == 2);
    }
    test(num == 2);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing double instance loop with double pointers... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	n1->left = n2;
	n2->left = n1;
	n1->right = n2;
	n2->right = n1;
	test(num == 2);
	Ice::collectGarbage();
	test(num == 2);
    }
    test(num == 2);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing double instance loop with looped pointers... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	n1->left = n2;
	n2->left = n1;
	n1->right = n1;
	n2->right = n2;
	test(num == 2);
	Ice::collectGarbage();
	test(num == 2);
    }
    test(num == 2);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing triple instance loop... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	n1->left = n2;
	n2->left = n3;
	n3->left = n1;
	test(num == 3);
	Ice::collectGarbage();
	test(num == 3);
    }
    test(num == 3);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing triple instance loop with double pointers... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	n1->left = n2;
	n2->left = n3;
	n3->left = n1;
	n1->right = n2;
	n2->right = n3;
	n3->right = n1;
	test(num == 3);
	Ice::collectGarbage();
	test(num == 3);
    }
    test(num == 3);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing triple instance loop with opposing pointers... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	n1->left = n2;
	n2->left = n3;
	n3->left = n1;
	n1->right = n3;
	n2->right = n1;
	n3->right = n2;
	test(num == 3);
	Ice::collectGarbage();
	test(num == 3);
    }
    test(num == 3);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing loop with trailing instances... " << flush;
    NPtr n;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	NPtr n4 = new N;
	n1->left = n2;
	n2->left = n1;
	n2->right = n3;
	n3->left = n4;
	n = n3;
	test(num == 4);
	Ice::collectGarbage();
	test(num == 4);
    }
    test(num == 4);
    Ice::collectGarbage();
    test(num == 2);
    n = 0;
    test(num == 0);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing loop with trailing instances and trailing loop... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	NPtr n4 = new N;
	NPtr n5 = new N;
	NPtr n6 = new N;
	n1->left = n2;
	n2->left = n1;
	n2->right = n3;
	n3->left = n4;
	n4->right = n5;
	n5->right = n6;
	n6->right = n5;
	n = n4;
	test(num == 6);
	Ice::collectGarbage();
	test(num == 6);
    }
    test(num == 6);
    Ice::collectGarbage();
    test(num == 3);
    n = 0;
    test(num == 2);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing for race conditions... " << flush;
    ::IceUtil::Time t = ::IceUtil::Time::now();
    int seed = argc > 1 ? atoi(argv[1]) : static_cast<int>(t.toMilliSeconds());
    ofstream file("seed");
    file << seed << "\n";
    srand(seed);

    typedef ::IceUtil::Handle<GarbageProducer> GarbageThreadPtr;
    GarbageThreadPtr garbageThread = new GarbageProducer();
    garbageThread->start();

    for(int i = 0; i < 50; ++i)
    {
	if(!interrupted())
	{
	    garbageThread->randomWait();
	    Ice::collectGarbage();
	}
    }

    garbageThread->stop();
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    MyApplication app;
    return app.main(argc, argv);
}
