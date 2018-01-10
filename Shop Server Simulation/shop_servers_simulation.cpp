#include<iostream>
#include<fstream>
#include<iomanip>
using namespace std;

const int MAX_SERVERS = 20;		//Max number of servers allowed
const int EVENTQ_SIZE = 1000;	// Event Priority Queue maximum size
const int CUSTQ_SIZE = 1000;	// Customer Queue maximum size

struct server
{
	double server_eff;			// Server Efficiency
	double cust_finish_time;	// Customer finish time
	bool busy;					// busy flag for server 
	int customer_served;		// Total number of customer served by a server
	double total_service_time; 	// Total service time of a server
	server()
	{
		busy = false;			// idle
		total_service_time = 0;
	}
};

struct event_queue
{
	int event_type;			// Event type checks customer arriving or customer completing payment
	double event_time;		// Customer arrival time when it is pushed into event queue
	double service_time;	// Customer service time

};

struct cust_queue
{
	double arr_time;		// Customer arrival time when it is read from the file
	double service_time;	// Customer service time
};

class Shop
{
	private:
	
	int no_of_servers;			   // Total number of servers specified by the user	
	server iservers[MAX_SERVERS];  // Heap of idle servers
	server bservers[MAX_SERVERS];  // Heap of busy servers
	
	event_queue eventq[EVENTQ_SIZE];	// Event Queue
	int eventq_count;					// Size of Event queue
	
	cust_queue custq[CUSTQ_SIZE];		// Customer Queue
	int custq_front, custq_rear;		// Size of Customer queue
	
	// Variables used for calculating stats
	double avg_queue_length; 	// Used for calculating average queue length and average customer waiting time
	int customer_arrival;		// Customer arrival event flag  
	int no_of_customers;		// Total number of customers
	double time_now;				// Cuurent time
	int greatestlength;			// Greatest length of the customer queue reached
	

	int heapsize_iserver;		// Heap size of idle servers
	int heapsize_bserver;		// Heap size of busy servers
	
	public:
	
	Shop()
	{
		customer_arrival = -1;
		avg_queue_length = 0;
		eventq_count = 0;
		no_of_customers = 0;
		custq_front = -1;
		custq_rear = -1;
		time_now = 0;
		greatestlength = 0;
		heapsize_iserver = 0;
		heapsize_bserver = 0;
	}
	void simulation();		// Function for starting simulation
	
	void enqueue_custq(double, double);		// Puts customer into customer queue
	bool IsEmpty_custq();					// Checks whether customer is empty or not
	cust_queue dequeue_custq();				// Remove customer from customer queue
	int custq_size();						// Finds the size of the customer queue

	void insertheap_event(event_queue*, event_queue, int&);		// Puts customer into event priority queue (heap) 
	void minheapify(event_queue*, int, int&);					// Function to perfom shift down operation on heap to keep the minimum at root 
	void swap(event_queue*, event_queue*);						// Function for swaping event queue elements
	event_queue extractmin(event_queue*, int&);					// Removes the customer from priority queue having least event time
	
	void insertheap_servers(server*, server, int&);				// Puts servers into idle server queue (heap) whose busy flag is false 
	void minheapify(server*, int, int&);						// Function to perfom shift down operation on heap to keep the minimum at root
	server extractmin(server* s, int&);							// Removes the server from heap having least server efficiency
	
	void insertheap_bservers(server*, server, int&);			// Puts servers into busy server queue (heap) whose busy flag is true
	void minbheapify(server*, int, int&);						// Function to perfom shift down operation on heap to keep the minimum at root
	server extractbmin(server*, int&);							// Removes the server from heap having least customer finish time
	
	void swap(server*, server*);								// Function for swaping server queue elements
};


int main()
{
	Shop shopname;
	shopname.simulation();	// Start Simulation
		
	return 0;
}

void Shop :: simulation()		// Function for starting simulation
{
	fstream file;
	char filename[20];                		// Variable for storing filename 
	event_queue eq;
	double  finish_time, busy_time;
	cust_queue nextavailablecust;
		
	cout << setprecision(6);
	
	cout<<"Enter file name:"<<endl;
	cin>>filename;
		
	file.open(filename);    //opening file
		
	if(file)       			// till eof
	{
		file >> no_of_servers;                    // read number of servers from file

		for(int i=0; i<no_of_servers; i++)
		{
			file >> iservers[i].server_eff;
			insertheap_servers(iservers,iservers[i],heapsize_iserver);
		}
		
			
		file >> eq.event_time >> eq.service_time;	//read first customer from the file
		eq.event_type = -1;
		insertheap_event(eventq, eq, eventq_count);	// Store first customer in event queue (heap)

		//While event priority queue not empty	
		while ( eventq_count > 0 ) 
		{
			eq = extractmin(eventq,eventq_count); // Extract customer from event queue (heap) having smallest event time
			
			time_now = eq.event_time;	// Store current time
			
			if(customer_arrival == eq.event_type)  // if event is -1 -->> Customer Arrival else server finishing
			{
				//Adding customer to customer FIFO queue
				enqueue_custq(eq.event_time, eq.service_time);
							
				//Read next Customer from file
				file >> eq.event_time >> eq.service_time;
				eq.event_type = -1;
			
				//If not EOF add Event to event priority queue
				if(file)   insertheap_event(eventq, eq, eventq_count);
							
				no_of_customers++; 	// count number of customers	
			}
			else
			{
				bservers[0].busy = false;  // set server idle
				insertheap_servers(iservers,extractbmin(bservers,heapsize_bserver),heapsize_iserver);	// extract busy server from busy server heap and push into idle server heap
			}
		
			
			//if customer FIFO not empty and idle server available
			if( !IsEmpty_custq() > 0  && heapsize_iserver > 0) 
			{
				//Get Next Customer from FIFO
				nextavailablecust = dequeue_custq();
					
				//Take most efficient idle server from the idle server heap and make it busy
				iservers[0].busy = true;
							
				//calculate server’s finish time (and do its stats)...
				iservers[0].customer_served++;
				busy_time = iservers[0].server_eff * nextavailablecust.service_time;
						
				avg_queue_length += (time_now-nextavailablecust.arr_time); 
				iservers[0].total_service_time +=  busy_time;	
				finish_time = time_now + busy_time;
				iservers[0].cust_finish_time = finish_time;

 				//add CustomerCompletePayment event to priority queue
 				insertheap_bservers(bservers,extractmin(iservers,heapsize_iserver),heapsize_bserver);
 	
				
				eq.event_time = finish_time;
				eq.service_time = nextavailablecust.service_time;
				eq.event_type = 0;
				insertheap_event(eventq, eq, eventq_count);		// Put customer back to event queue with updated event time 
			}		
		}
		
	cout << "Number of customers served: " << no_of_customers << endl; 
	cout << "Time last customer completed service: " << time_now << endl;
	cout << "Greatest length reached by the queue: " << greatestlength << endl;
	cout << "Average length of the queue: "<< avg_queue_length/time_now<< endl;
	cout << "Average customer waiting time: " << avg_queue_length/no_of_customers << endl;
	
	cout << "Server" << setw(15) <<"Priority" << setw(20) << "CustomerServed" << setw(15) << "IdleTime" << endl;
	
	for(int i=0; i<no_of_servers; i++)
	{
		cout << setw(3) << i << setw(17) << iservers[i].server_eff << setw(15) << iservers[i].customer_served << setw(20) << time_now - iservers[i].total_service_time <<  endl;
	}
	 
			
	}
	else
	{
		cerr << "File not found... Unable to read file" <<endl;
	}
	
	file.close();
}

void Shop :: enqueue_custq(double atime, double stime)		// Puts customer into customer queue
{
	int size;
	if (IsEmpty_custq())		// If Customer queue is empty
	{ 
		custq_front = custq_rear = 0; 
	}
	else
	{
	    custq_rear = (custq_rear+1)%CUSTQ_SIZE;
	}
	
	size = custq_size();

	if(greatestlength < size)
		greatestlength = size;		// Finding greatest customer queue length
	
	custq[custq_rear].arr_time = atime;
	custq[custq_rear].service_time = stime;
}
bool Shop :: IsEmpty_custq()		// Checks whether customer is empty or not
{
	return (custq_front == -1 && custq_rear == -1); 
}
cust_queue Shop :: dequeue_custq()	// Remove customer from customer queue
{
	int temp = custq_front;
	if(custq_front == custq_rear ) 		// If queue contains one element
	{
		custq_rear = custq_front = -1;
	}
	else
	{
		custq_front = (custq_front+1)%CUSTQ_SIZE;
	}
	return custq[temp];
}
int Shop :: custq_size()		// Finds the size of the customer queue
{
	if(IsEmpty_custq())
		return 0;
	return custq_rear - custq_front + 1; 
}
void Shop :: insertheap_event(event_queue* event, event_queue e, int& size)		// Puts customer into event priority queue (heap)
{
	int i, parent;
	if (size == EVENTQ_SIZE)
	{
		cout << "Event queue exceeding limit" << endl;
		exit(0);
	}
		
	size++;	
	i = size - 1;
	event[i] = e;
	
	parent = (i-1)/2;
	while(i != 0 && event[parent].event_time >= event[i].event_time)
	{
		swap(&event[i],&event[parent]);
		i = parent;
		parent = (i-1)/2;
	}
}
void Shop :: minheapify(event_queue* event, int i, int& size)		// Function to perfom shift down operation on heap to keep the minimum event at root 
{
	int l = 2*i + 1;
    int r = 2*i + 2;
    int smallest = i;
	if(l < size && event[l].event_time <= event[i].event_time)
		smallest = l;
	if (r < size && event[r].event_time <= event[smallest].event_time)
		smallest = r;
	if (smallest != i)
    {   
        swap(&event[i], &event[smallest]);
        minheapify(event,smallest,size);
    }
}
void Shop :: swap(event_queue* x, event_queue* y)		// Function for swaping event queue elements
{
	event_queue temp = *x;
    *x = *y;
    *y = temp;
}
event_queue Shop :: extractmin(event_queue* event, int& size)	// Removes the customer from priority queue having least event time
{
	if (size == 1)
    {
        size--;
        return event[0];
    }
 
    // Store the minimum value, and remove it from heap
    event_queue root = event[0];
    event[0] = event[size-1];
    size--;
    minheapify(event,0,size);
 
    return root;
}
void Shop :: insertheap_servers(server *id,server s,int &size)	// Puts servers into idle server queue (heap) whose busy flag is false 
{
	int i, parent;
	if (size == MAX_SERVERS)
	{
		cout << "Servers exceeding limit" << endl;
		exit(0); 
	}
		
	size++;	
	i = size - 1;
	id[i] = s;
	
	parent = (i-1)/2;
	while(i != 0 && id[parent].server_eff >= id[i].server_eff)
	{
		swap(&id[i],&id[parent]);
		i = parent;
		parent = (i-1)/2;
	}
}
void Shop :: minheapify(server *s,int i,int &size)	// Function to perfom shift down operation on heap to keep the most efficient server at root
{
    int l = 2*i + 1;
    int r = 2*i + 2;
    int smallest = i;
	if(l < size && s[l].server_eff <= s[i].server_eff)
		smallest = l;
	if (r < size && s[r].server_eff <= s[smallest].server_eff)
		smallest = r;
	if (smallest != i)
    {   
        swap(&s[i], &s[smallest]);
        minheapify(s,smallest,size);
    }
}
server Shop :: extractmin(server* s,int &size)		// Removes the server from heap having least server efficiency
{
	if (size == 1)
    {
        size--;
        return s[0];
    }
 
    // Store the minimum value, and remove it from heap
    server root = s[0];
    s[0] = s[size-1];
    size--;
    minheapify(s,0,size);
 
    return root;
}

void Shop :: insertheap_bservers(server *idser,server s,int &size)	// Puts servers into busy server queue (heap) whose busy flag is true
{
	int i, parent;
	if (size == MAX_SERVERS)
	{
		cout << "Servers exceeding limit" << endl;
		exit(0); 
	}
		
	size++;	
	i = size - 1;
	idser[i] = s;
	
	parent = (i-1)/2;
	while(i != 0 && idser[parent].cust_finish_time >= idser[i].cust_finish_time)
	{
		swap(&idser[i],&idser[parent]);
		i = parent;
		parent = (i-1)/2;
	}
}
void Shop :: minbheapify(server *s,int i,int &size)		// Function to perfom shift down operation on heap to keep that busy server at root which got finished early
{
    int l = 2*i + 1;
    int r = 2*i + 2;
    int smallest = i;
	if(l < size && s[l].cust_finish_time <= s[i].cust_finish_time)
		smallest = l;
	if (r < size && s[r].cust_finish_time <= s[smallest].cust_finish_time)
		smallest = r;
	if (smallest != i)
    {   
        swap(&s[i], &s[smallest]);
        minbheapify(s,smallest,size);
    }
}
server Shop :: extractbmin(server* s,int &size)		// Removes the server from heap having least customer finish time
{
	if (size == 1)
    {
        size--;
        return s[0];
    }
 
    // Store the minimum value, and remove it from heap
    server root = s[0];
    s[0] = s[size-1];
    size--;
    minbheapify(s,0,size);
 
    return root;
}
void Shop :: swap(server *x, server *y)		// Function for swaping server queue elements
{
    server temp = *x;
    *x = *y;
    *y = temp;
}


