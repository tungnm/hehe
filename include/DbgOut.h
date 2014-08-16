#ifndef DbgOut_h__
#define DbgOut_h__

struct outstr //Base class out string
{
public:
	outstr()
	{
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	template<class T>
	outstr &operator<<(const T &x) {
		m_SS << x;
		return *this;
	}
protected:
	std::ostringstream m_SS;
	HANDLE  hConsole;
	
};

struct debug : public outstr  //Regular debug
{
	virtual ~debug() {
		std::cerr << m_SS.str() << std::endl;
	}
};


struct dCritical : public outstr //Critical debug 
{
	~dCritical() {
		SetConsoleTextAttribute(hConsole, 12 /*Red*/);
		std::cerr << m_SS.str() << std::endl;
		SetConsoleTextAttribute(hConsole, 15 /*White*/);
	}
};


struct dWarning : public outstr //Critical debug 
{
	~dWarning() {
		SetConsoleTextAttribute(hConsole, 14 /*Yellow*/);
		std::cerr << m_SS.str() << std::endl;
		SetConsoleTextAttribute(hConsole, 15 /*White*/);
	}
};


#endif
