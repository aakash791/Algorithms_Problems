/*
This program does the following:
1. Read the name of the text file from the console.
2. Read in a text file, line by line.
3. Split each line into words, discarding punctuation and folding all letters into
lower case.
4. Store the unique words and maintain a count of each different word.
5. Sort the words first by decreasing count and, if there are multiple words with
the same count, alphabetically.
6. Output the first ten words in the sorted list, along with their counts.
7. Output the last ten words in the list, along with their counts.
*/
#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<cstring>
#include<cctype>

const int MAX=50000;    //Max number of words in a file  
const int MAXC=1000000; //Max number of characters allowed to store in a string pool
using namespace std;

char pool[MAXC]; 		// String Pool

struct node				//Tree node		
{
	int start;			// Variable for storing start index of word
	int last;			// Variable for last index of word
	int count;			// Variable for storing count of word 
	int left;			// Variable for storing left child index
	int right;			// Variable for storing right child index
} tree[MAX],*btree[MAX];  // tree variables

int tindex = 0, ctindex = 0;  //size of trees

void createnode(int, int);      	//Creating bst node
void createbst(int, int); 			//Creating bst search tree for sorting words in alphabetical order
void createbstcount(node*);			//Creating bst node		
void createnodecount(node*);		//Creating bst search tree again for sorting words based on its count, if counts are equal storing in alphabetical order
void display(int, int);				//Display first and last words and their counts 
void getString(int, int);			//Display word using start and end index
int compare(int ,int ,int ,int);    //Comparing words

int main()
{
	char filename[20];                		// Variable for storing filename 
	
	string line,word;                       // each word from the file will be stored in line variable
	cout<<"Enter file name:"<<endl;
	cin>>filename;
	
	fstream file;
	file.open(filename);    //opening file
	
	int i,start,end,pool_index=0;    				//loop variables
	if(file)       			// till eof
	{
		while(getline(file,line))          //read input line by line
		{  
			stringstream ss(line);               
			while(ss>>word)                    // read each word from that line
			{
				start = pool_index;				// store start index of the word
				for(i=0; i<word.length(); i++)
				{
					word[i]=tolower(word[i]);
					if(!ispunct(word[i]))      // check if word contains any punctuations or not 
					{
						pool[pool_index] = word[i];
						pool_index++;                    // if yes, remove that punctuation from the word       
					} 
				}		
				
				end = pool_index-1;				//store last index of the word
				createbst(start,end);   		//Insert word in a tree, to store the words alphabetically and keep their count 
				tindex++;
		   }
		}
		
		
		for(i=0;i<tindex;i++)
		{
			if(tree[i].count!=0)
			{
				createbstcount(&tree[i]);   //Insert a tree node, to sort the words comparing their counts
				ctindex++;
			}
		}
		
		display(0,10);   // 10 specifies the number of words to be printed
		
	}
	else
	{
		cerr<<"unable to read file or file does not exist"<<endl;    //display error message
	}
	
	file.close();    //closing file
	
	
	
	return 0;
}

void createnode(int start,int end)		//Creating a new node
{
	tree[tindex].count++;
	tree[tindex].start = start;
	tree[tindex].last = end;
	tree[tindex].left = tree[tindex].right = -1;
}
void createbst(int start,int end)     //Function for creating bst search tree for sorting words in alphabetical order   
{
	int bindex = 0;
	
	while(bindex <= tindex)
	{
		if(tindex == 0)         // Creating root node
		{
			createnode(start,end);
			return;
		}
		if( compare(start,end,tree[bindex].start, tree[bindex].last) < 0)   //  if a new node word comes alphabetically before its parent node word, make new node its left child  
		{
			if(tree[bindex].left == -1)
			{
				tree[bindex].left = tindex;
				createnode(start,end);
				return;
			}
			else
			{
				bindex = tree[bindex].left;
				continue;
			}
			
		}
		else if( compare(start,end,tree[bindex].start, tree[bindex].last) > 0)  // if a new node word comes alphabetically after its parent node word, make new node its right child 
		{
			if(tree[bindex].right == -1)
			{
				tree[bindex].right = tindex;
				createnode(start,end);
				return;
			}
			else
			{
				bindex = tree[bindex].right;
				continue;
			}
		}
		else		 // if a new node word is equal to its parent node word increment its count by 1 
		{
			tree[bindex].count++;
			tindex--;
			break;
		}
	}
}

void createnodecount(node *t)    // //Creating a new node
{
	btree[ctindex]=t;
	btree[ctindex]->left = btree[ctindex]->right = -1;
}
void createbstcount(node *t)    // Function for creating bst search tree again for sorting words based on its count, if counts are equal store in alphabetical order 
{
	int bindex = 0;
	
	while(bindex <= ctindex)	
	{
		if(ctindex == 0) 		// Creating root node
		{
			createnodecount(t);
			return;
		}
		if(t->count > btree[bindex]->count )   // if new node count is greater than its parent node count, make new node its left child 
		{
			if(btree[bindex]->left == -1)
			{
				btree[bindex]->left = ctindex;
				createnodecount(t);
				return;
			}
			else
			{
				bindex = btree[bindex]->left;	
				continue;
			}
			
		}
		else if(t->count < btree[bindex]->count)	// if new node count is less than its parent node count, make new node its right child
		{
			if(btree[bindex]->right == -1)
			{
				btree[bindex]->right = ctindex;
				createnodecount(t);
				return;
			}
			else
			{
				bindex = btree[bindex]->right;
				continue;
			}
		}
		else										// if new node count is equal to its parent node count, compare the words 
		{
			if(compare(t->start,t->last,btree[bindex]->start,btree[bindex]->last) <= 0)	 // if a new node word comes alphabetically before its parent node word, make new node its left child 
			{
				if(btree[bindex]->left == -1)
				{
					btree[bindex]->left = ctindex;
					createnodecount(t);
					return;
				}
				else
				{
					bindex = btree[bindex]->left;
					continue;
				}
				
			}
			else 										 // if a new node word comes alphabetically after its parent node word, make new node its right child  
			{
				if(btree[bindex]->right == -1)
				{
					btree[bindex]->right = ctindex;
					createnodecount(t);
					return;
				}
				else
				{
					bindex = btree[bindex]->right;
					continue;
				}
			}	
		}
	}
}
void display(int index,int limit)		// Function for displaying first and last 10 words and their counts
{
	static int check = 1;
	if(btree[index]->left != -1)			
	display( btree[index]->left ,limit);
	
	if(check==1) cout<<"First "<< limit <<" words and their counts:- "<<endl; 
	if(check <= limit)
	{
		cout<<setw(15);
		getString(btree[index]->start,btree[index]->last);
		cout<<setw(5)<<btree[index]->count<<endl;
	}
	if((ctindex-check)==limit) cout<<"Last "<< limit <<" words and their counts:- "<<endl;
	if((ctindex-check)< limit)
	{
		cout<<setw(15);
		getString(btree[index]->start,btree[index]->last);
		cout<<setw(5)<<btree[index]->count<<endl;		
	}
	check++;
	
	if(btree[index]->right != -1)
	display( btree[index]->right ,limit);
}
void getString(int start, int end)		// Function for fetching string from string pool and displaying
{
	int i,j;
	char str[30];
	for(i=start,j=0; i<=end; i++,j++)
		str[j] = pool[i];	
	str[j]='\0';
	cout << str;
}
int compare(int start,int end,int tstart, int tend)		// Function for comparing two strings by passing their start and end indexes
{
	int i,j;
	char str[30],tstr[30];
	
	for(i=start,j=0; i<=end; i++,j++)
		str[j] = pool[i];	
	str[j]='\0';
	
	for(i=tstart,j=0; i<=tend; i++,j++)
		tstr[j] = pool[i];	
	tstr[j]='\0';
		
	return strcmp(str,tstr);	
}

