#include<iostream>

using namespace std;

typedef struct List {

	int data;
	struct List* next; 

} Node;


class Stack {

	Node *node, *rear;
	
	public:
	
	Stack(){
		node = NULL;
		rear = NULL;	
	} 
	
	~Stack(){
		delete[] node;
	}
	
	void Push(int data){
		if (node ==  NULL){
			node = new Node;
			node->data = data;
			node->next = NULL;
			rear = node;
		}else{
			Node *temp = new Node;
			temp->data = data;
			temp->next = NULL;
			rear->next = temp;
			rear = temp;
		}
	}
	
	int Pop(){
		int val = -1;
		
		if (rear == NULL){
			cout << "STACK IS EMPTY" << endl;
		}else if( node == rear ){
			val = rear->data;
			delete rear;
			node = rear = NULL;
		}else{
			Node* temp = node;
			while(temp->next->next!=NULL){
				temp = temp->next;
			}
			
			temp->next = NULL;
			val = rear->data;
			delete rear;
			rear = temp;
		}
		return val;
	}

	void print(){
		Node *temp = node;
		
		if (temp == NULL){
			cout << "STACK IS EMPTY" << endl;
			return;
		}
		 
		while (temp!=NULL){
			cout << temp->data << " ";
			temp = temp->next;
		}
		cout << endl;
	}
};

int main(){
	
	
	Stack s;
	
	cout << "PUSH OPERATIONS" << endl;
	s.Push(1);
	s.Push(2);
	
	cout << "PRINT STACK" << endl;
	s.print();
	
	cout << "POP OPERATIONS" << endl;
	cout << s.Pop() << endl;
	cout << s.Pop() << endl;
	
	cout << "PRINT STACK" << endl;
	s.print();
	
	cout << "PUSH OPERATIONS" << endl;
	s.Push(2);
	
	cout << "PRINT STACK" << endl;
	s.print();
	
	cout << "POP OPERATIONS" << endl;	
	cout << s.Pop() << endl;
	cout << s.Pop() << endl;
	
	cout << "PRINT STACK" << endl;
	s.print();
	
	cout << endl << "********END***********" << endl;
 	
	return 0;
}
