//Michael Hettegger

#include <iostream>
#include <thread>

using namespace std;

enum launch {
	ASYNC,
	SYNC,
	DEFERRED
};

class myFuture {
	private:
		bool done;
		int	result;
		launch launchPolicy;
		function<void(void)> myFunction;
		thread* t1;

	public:
		myFuture(launch policy) : 
			done(false),
			result(0),
			launchPolicy(policy),
			t1(NULL) 
		{}
		
		~myFuture() {
			if(t1 != NULL)
				delete t1;				
		}

		static myFuture async(launch policy, int (*func) (int), int x) {
			
			myFuture myF(policy);

			//Lampda Function
			myF.myFunction = [func, &myF, x](){
				//saves the result of the function, in my case int
				myF.result=func(x);
				myF.done=true;
			};

			if(myF.launchPolicy == launch::SYNC)
				myF.myFunction();
			else if(myF.launchPolicy == launch::ASYNC)
				myF.t1 = new thread(myF.myFunction);

			return myF;
		} 
		
		int get() {
			if(launchPolicy == launch::ASYNC && !done)
				t1->join();
			else if(launchPolicy == launch::DEFERRED && !done)
				myFunction();

			return result;
		}	
};


int myPow(int x) {
	cout << "Calculating myPow"<<endl;
	this_thread::sleep_for(chrono::seconds(5));
	return x*x;
}

int main() {
	cout << "------------ ASYN ----------------" << endl;
	myFuture asynF = myFuture::async(launch::ASYNC, *myPow, 5);
	cout << "Cout after Future Start" << endl;
	cout << "Result: "<<asynF.get() << endl<<endl;

	cout << "------------ SYNC ----------------" << endl;
	myFuture synF = myFuture::async(launch::SYNC, *myPow, 5);
	cout << "Cout after Future Start" << endl;
	cout << "Result: "<< synF.get() <<endl<<endl;

	cout << "------------ DEFERRED ----------------" << endl;
	myFuture defF = myFuture::async(launch::DEFERRED, *myPow, 5);
	cout << "Cout after Future Start" << endl;
	cout << "Result: "<< defF.get() << endl<<endl;
	
	return 0;
}
