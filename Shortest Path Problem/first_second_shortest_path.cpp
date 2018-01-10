/*
Assignment 3 - Finding First and Second Shortest Path
Aakash Deep
ad930
*/
#include <iostream>
#include <fstream>
#include <cmath>
#define inf 9999999
using namespace std;

const int SIZE = 1000;  
struct vertex		// This struct will contain the vertex name, its X and Y coordinates
{
	char name;
	int x,y;
}; 

struct edge			// This struct will contain 2 endpoints of an edge, it weight and heuristics
{
	char v1, v2;
	int weight;
	int h;
}; 

struct heap			// This struct will store the index and weight of dist array used for astar algorithm
{
	int index;
	int weight;
}; 

struct path_heap	// This struct will store the index and weight of dist array used for astar algorithm
{
	int wght;
	int *p;
	int length;
}; 

int vertices, edges;  		// Storing total number of vertices and edges
int graph[SIZE][SIZE];		// Storing weight for each edge in graph
int parent[SIZE];			// Storing parent for each vertex
int path[SIZE], pindex = 0;	// Storing path 
char src, dest;				// Storing source and destination
int length = 0;				// Storing length of the path

vertex v[SIZE];				// Storing each vertex 
edge ed[SIZE];				// Storing each edge
heap hp[SIZE];				// Min heap of weights
path_heap pheap[SIZE];		// Min heap of paths and shortest distances
int size = 0, psize=0;		// Heap sizes

int notfound = 0;			// Flag, which suggests if there is a shortest path available or not 

int spath[SIZE][SIZE], sindex=-1;	// Will hold the shortest path found after removing each edge from the first shortest path
int dist[SIZE];				// Will hold the shortest distance from source for each vertex

void astar(int, int);	// Function for finding shortest path from single source and single destination using modified astar Algorithm (making use of heuristics)
void cal_heuristics();		// Function for calculating Euclidean distance for each vertex

void second_shortest(int, int);			// Function for finding second shortest distance 
void find_path(int[], int);			// Function for printing path for shortest distance

void insertheap(heap*, int, int, int&);	// Function for putting node into heap
void minheapify(heap*, int, int&);		// Function to perform shift down operation on heap to keep node with minimum weight at root
heap extractmin(heap*, int&);			// Function for removing node from heap having least weight
void swap(heap*, heap*);				// Function for swaping heap elements

void insertpheap(path_heap*,int ,int[] ,int , int&);		// Function for putting node into heap
void minpheapify(path_heap*,int , int&);					// Function to perform shift down operation on heap to keep the shortest distance and its path at root
path_heap extractpmin(path_heap*, int&);					// Function for removing node from heap having shortest distance
void pswap(path_heap*, path_heap*);							// Function for swaping heap elements

int main()
{
	char filename[20];
	
	cout<<"Enter file name:"<<endl;			
	cin>>filename;
	
	fstream file;
	file.open(filename);                 //opening file
	
	if(!file)
	{
		cout<<"Unable to open file";           //prints error message
		exit(0);
	}
	
	file >> vertices >> edges;				// Reads total number of vertices and edges from file
	
	for(int i=0; i<vertices; i++)
		for(int j=0; j<vertices; j++)
			graph[i][j] = inf;				// keeping each cell as infinity in adjacency matrix initially
	
	for(int i=0; i<vertices; i++)
		file >> v[i].name >> v[i].x >> v[i].y;	// Reads X and Y coordinates from file, for each vertex and storing in vertex struct node	
	
	for(int i=0; i<edges; i++)
	{
		file >> ed[i].v1 >> ed[i].v2 >> ed[i].weight;		// Reading and storing edges in a graph from file and storing in edge struct node
		graph[ed[i].v1-97][ed[i].v2-97] = ed[i].weight;		// Genearating adjacency matrix with weights for above edges
	}
	
	file >> src >> dest;		// Reading source and destination vertex from file
	int sr = src-97;			
	int dr = dest-97;
	
	cal_heuristics();			// Calculating Euclidean distance for each vertex
	
	/* Printing First Shortest Path  */
	
	cout << "First Shortest Path from " << src << " to " << dest << endl;
	cout << "---------------------------------------" << endl;
	astar(sr,dr);		// Calling astar to find first shortest path
	if(notfound == 0)
	{
		cout <<"Shortest Distance: " << dist[dr] << endl;	// Prints shortest distance
		cout << "Shortest Path: ";							// Prints shortest path
		
	  	path[pindex] = sr;			// storing the first vertex as source in the path
	  	pindex++;
	    find_path(parent, dr);			// Finds and stores the shortest path by calling the find_path function
	    for(int i = 0; i<length; i++)
	    {
	    	cout << (char)(path[i] + 97) ;
			if(i != length -1)
			cout << "-->" ;
		}
	    cout << endl << "Length of the path: " << length << endl << endl;	// Prints length of the path
	}
	else
   	{
   		cout << "No first shortest path found" << endl;
	}
	
    /* Printing Second Shortest Path  */
    
    cout << "Second Shortest Path from " << src << " to " << dest << endl;
    cout << "---------------------------------------" << endl;
    second_shortest(sr,dr);			// finds second shortest path
	
	path_heap temp = extractpmin(pheap, psize);		// Extract the shortest of the second shortest distance and path
	
	if(notfound == 0)
	{
		cout << "Shortest Distance: " << temp.wght << endl;			// Prints shortest distance
		cout << "Shortest Path: ";									// Prints shortest path
	
		for(int i=0;i < temp.length; i++)
		{
			cout << (char)(temp.p[i] + 97);
			if(i != temp.length -1)
				cout << "-->" ;
		}	   
   		cout << endl << "Length of the path: " << temp.length << endl;	// Prints length of the path
   	}
   	else
   	{
   		cout << "No second shortest path found" << endl;
	}
	file.close(); // closing file
	return 0;
}
void astar(int source, int target)	// Function for finding shortest path from single source and single destination using modified astar Algorithm
{
	for (int i = 0; i < vertices; i++)
    	dist[i] = inf;					// Keeping all distances initially to infinity
    
	parent[source] = -1;	// Keeping parent of source as -1
    dist[source] = 0;		// Keeping distance from source to source as 0 
  	
	insertheap(hp,source,dist[source],size);   	// Storing index and weight stored at index in dist array into the heap
    for (int i = 0; i < vertices; i++)
    {
    	
    	heap min_ind = extractmin(hp,size);		// Fetching root node having minimum weight in the heap
    	int u = min_ind.index;					// and storing its index
    	
        if( i == target && dist[target] != inf)					// Break the loop if destination is found and its not infinity
      		break;
	
		if(i == vertices - 1)	// If no path is found between source and vertex
			notfound = 1;
	
       	for (int v = 0; v < vertices; v++)
       	{
       	 if ( graph[u][v] && dist[u]+graph[u][v] < dist[v] )  //If there is an edge between u and v, and total weight of path from source to v is smaller than current value of dist[v]
         {
         	dist[v] = dist[u] + graph[u][v];	// Update dist[v]
           	insertheap(hp,v,dist[v],size);		// Inserting index and updated weight stored at v in dist array, into the heap
         	parent[v] = u; 						// Assigning parent for current node
		 }   
   		}
    }
}
void cal_heuristics()		// Function for calculating Euclidean distance for each vertex
{
	for(int i=0; i <vertices; i++)
		ed[i].h = sqrt( pow(double(v[dest-97].x - v[i].x),2.0) + pow(double(v[dest-97].y - v[i].y),2.0) );
}
void second_shortest(int source, int target)	// Function for finding second shortest distance
{
	int j=pindex;
	sindex = 0;
	int fpath[SIZE];
	for(int i=0; i<j; i++)
		fpath[i] = path[i];
	for(int i=0; i<j-1; i++)		
	{
		notfound = 0;
		length = 0;
		int temp = graph[fpath[i]][fpath[i+1]];
		graph[fpath[i]][fpath[i+1]] = inf;	// putting the each edge as infinity in the first shortest path
	
		astar(source,target);	// calling astar again
		if(notfound == 0)
		{
			spath[sindex][0] = source;

		  	pindex=0;
		  	path[pindex] = source;		// storing the first vertex as source in the path
		  	pindex++;
		    find_path(parent, target);	// Finds and stores shortest path by calling find_path function

		    insertpheap(pheap, dist[target], spath[sindex], length, psize);		// Keep on inserting the shortest distance, shortest path, and its length into the heap 
		    sindex++;		    
		}
		
		graph[fpath[i]][fpath[i+1]] = temp;		// Assign the original value to the previous edge
		
	}
	

}
void find_path(int parent[], int j)	// Function for printing path for shortest distance
{
	static int m=1;
	length++;		// Calculating length of the path

    if (parent[j]==-1)		// Break the function once parent of vertex reaches -1
    {
    	m = 1;
    	return;
	}
        
 	
    find_path(parent, parent[j]);		// call recursively with parent array
 	
 	if(sindex != -1)
 	{
 		spath[sindex][m] = j;			// Stores every path found, in spath array
		m++; 
	}
 	
 	path[pindex] = j;
 	pindex ++;
}
void insertheap(heap *que,int index, int value, int &size)	// Put node into heap which stores the index as well as the updated weight (using heuristics)
{
	int i, parent;
	if (size == SIZE)
	{
		cout << "Heap exceeding limit" << endl;
		exit(0); 
	}
		
	size++;	
	i = size - 1;
	que[i].index = index;					// Adding index in heap 
	que[i].weight = value + ed[index].h;	// Adding heuristic to the weight and storing updated weight in heap
	
	parent = (i-1)/2;
	while(i != 0 && que[parent].weight >= que[i].weight)		// Rearranging heap nodes based on weights
	{
		swap(&que[i],&que[parent]);
		i = parent;
		parent = (i-1)/2;
	}
}
void minheapify(heap *que,int i,int &size)		// Function to perfom shift down operation on heap to keep node with minimum weight at root
{
    int l = 2*i + 1;
    int r = 2*i + 2;
    int smallest = i;
	if(l < size && que[l].weight <= que[i].weight)
		smallest = l;
	if (r < size && que[r].weight <= que[smallest].weight)
		smallest = r;
	if (smallest != i)
    {   
        swap(&que[i], &que[smallest]);
        minheapify(que,smallest,size);
    }
}
heap extractmin(heap* que,int &size)		// Removes node from heap having least weight
{
	if (size == 1)
    {
        size--;
        return que[0];
    }
 
    // Store the node having minimum weight, and remove it from heap
    heap root = que[0];
    que[0] = que[size-1];
    size--;
    minheapify(que,0,size);
 
    return root;
}
void swap(heap *x, heap *y)		// Function for swaping heap elements
{
    heap temp = *x;
    *x = *y;
    *y = temp;
}

void insertpheap(path_heap *que,int index, int value[],int len, int &size)	// Puts node into heap which stores all the shortest distance generated while finding the second shortest path
{
	int i, parent;
	if (size == SIZE)
	{
		cout << "Heap exceeding limit" << endl;
		exit(0); 
	}
		
	size++;	
	i = size - 1;
	que[i].wght = index;
	que[i].p = value;
	que[i].length = len;
	
	parent = (i-1)/2;
	while(i != 0 && que[parent].wght >= que[i].wght)
	{
		pswap(&que[i],&que[parent]);
		i = parent;
		parent = (i-1)/2;
	}
}
void minpheapify(path_heap *que,int i,int &size)		// Function to perfom shift down operation on heap to keep the node with the shortest distance at root
{
    int l = 2*i + 1;
    int r = 2*i + 2;
    int smallest = i;
	if(l < size && que[l].wght <= que[i].wght)
		smallest = l;
	if (r < size && que[r].wght <= que[smallest].wght)
		smallest = r;
	if (smallest != i)
    {   
        pswap(&que[i], &que[smallest]);
        minpheapify(que,smallest,size);
    }
}
path_heap extractpmin(path_heap* que,int &size)		// Removes node from the heap having shortest distance
{
	if (size == 1)
    {
        size--;
        return que[0];
    }
 
    // Store the minimum value, and remove it from heap
    path_heap root = que[0];
    que[0] = que[size-1];
    size--;
    minpheapify(que,0,size);
 
    return root;
}
void pswap(path_heap *x, path_heap *y)		//  Function for swaping heap elements
{
    path_heap temp = *x;
    *x = *y;
    *y = temp;
}

