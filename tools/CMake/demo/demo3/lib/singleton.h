#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton
{
	public:
		static T& GetInstance()
		{
			static T instance;
			return instance;
		}
		Singleton(Singleton const &) = delete;
		Singleton& operator=(Singleton const &) = delete;

	protected:
		Singleton() = default;
		~Singleton() = default;

};

#endif
