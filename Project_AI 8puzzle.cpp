
#include <iostream>

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<cstdlib>
#include<ctime>
#include<time.h>
#include<random>
#include<Windows.h>

#define D 16
enum class Actions {
	left, right, up, down

};
std::string action_names[] = { "left","right","up","down" };
#define SIDE 3 // Length of the board 
typedef struct Node_array
{
	int state[SIDE][SIDE];
	Actions action;
	struct Node_array* parent; // tree search

	int path_cost;
	int depth;
	struct Node_array* left, * right, * up, * down, * nextNode;
	double score;

} Node;

typedef struct move_array
{

	Actions action;
	double score = 0;
	int state[SIDE][SIDE];
	struct move_array* left, * right, * up, * down;

} Move;

typedef struct NodeList {
	unsigned int nodeCount;    //the number of nodes in the list
	Node* head;            //pointer to the first node in the list
	Node* tail;            //pointer to the last node in the list
};

NodeList* Initial()// khởi tạo một danh sách liên kết mà các node là các nút đã đi
{
	NodeList* List;
	List = (NodeList*)malloc(sizeof(NodeList));
	List->nodeCount = 0;
	List->head = NULL;
	List->tail = NULL;
	return List;
}



void copy_array(int a[][SIDE], int b[][SIDE])//copy b to a
{
	int i, j;
	for (i = 0; i < SIDE; i++)
		for (j = 0; j < SIDE; j++)
			a[i][j] = b[i][j];
}

int is_equal(int a[][SIDE], int b[][SIDE])
{
	int i, j, flag = 1;
	for (i = 0; i < SIDE; i++)
		for (j = 0; j < SIDE; j++)
			if (a[i][j] != b[i][j])
				flag = 0;
	return flag;
}

void swap(int& a, int& b)
{
	int temp;
	temp = a;
	a = b;
	b = temp;
}

void print_array(int a[][SIDE])
{
	int i, j;
	printf("\n");
	for (i = 0; i < SIDE; i++)
	{
		for (j = 0; j < SIDE; j++)
			printf("%d  | ", a[i][j]);
		printf("\n");
		for (j = 0; j < SIDE; j++)
			printf("---|-");

		printf("\n");
	}
	printf("\n");
}

bool isLegalAction(Node* node, Actions action) {
	int index_i;
	int index_j;
	for (int i = 0; i < SIDE; i++) {
		for (int j = 0; j < SIDE; j++) {
			if (node->state[i][j] == 0) {
				index_i = i;
				index_j = j;
				i = SIDE;
				break;
			}
		}
	}
	if (action == Actions::left && index_j == 0)  return false;
	if (action == Actions::right && index_j == 2)  return false;

	switch (index_i)
	{
	case 0:
		if (action == Actions::up) return false;
		return true;
	case 2:
		if (action == Actions::down) return false;
		return true;
	default:
		return true;
	}
	return true;
}

Node* Child_node(Node*& node, Actions action) {
	Node* child = (Node*)malloc(sizeof(Node));
	int index_i, index_j;
	for (int i = 0; i < SIDE; i++) {
		for (int j = 0; j < SIDE; j++) {
			if (node->state[i][j] == 0) {
				index_i = i;
				index_j = j;
				i = SIDE;
				break;
			}
		}
	}
	copy_array(child->state, node->state);
	child->action = action;
	child->parent = node;
	child->path_cost = node->path_cost + 1;
	if (action == Actions::left) {
		swap(child->state[index_i][index_j - 1], child->state[index_i][index_j]);
	}
	else if (action == Actions::right) {
		swap(child->state[index_i][index_j + 1], child->state[index_i][index_j]);
	}
	else if (action == Actions::up) {
		swap(child->state[index_i - 1][index_j], child->state[index_i][index_j]);
	}
	else if (action == Actions::down) {
		swap(child->state[index_i + 1][index_j], child->state[index_i][index_j]);
	}
	return child;
}

Node* NewNode(Node* node, Actions action)
{
	Node* newnode = (Node*)malloc(sizeof(Node));
	int index_i, index_j;
	for (int i = 0; i < SIDE; i++) {
		for (int j = 0; j < SIDE; j++) {
			if (node->state[i][j] == 0) {
				index_i = i;
				index_j = j;
				i = SIDE;
				break;
			}
		}
	}
	for (int i = 0; i < SIDE; i++)
		for (int j = 0; j < SIDE; j++)
			newnode->state[i][j] = 0;
	copy_array(newnode->state, node->state);
	if (action == Actions::left) {
		swap(newnode->state[index_i][index_j - 1], newnode->state[index_i][index_j]);
	}
	else if (action == Actions::right) {
		swap(newnode->state[index_i][index_j + 1], newnode->state[index_i][index_j]);
	}
	else if (action == Actions::up) {
		swap(newnode->state[index_i - 1][index_j], newnode->state[index_i][index_j]);
	}
	else if (action == Actions::down) {
		swap(newnode->state[index_i + 1][index_j], newnode->state[index_i][index_j]);
	}
	return newnode;
}
Node* createNode(Node* node)
{
	Node* child = new Node();
	copy_array(child->state, node->state);
	child->action = node->action;
	child->depth = node->depth;
	child->nextNode = node->nextNode;
	child->parent = node->parent;
	child->path_cost = node->path_cost;
	return child;
}
void enqueue(NodeList* list, Node* node)
{
	if (list->nodeCount <= 0)
	{
		list->head = node;
		list->tail = node;
		list->nodeCount += 1;
		return;
	}
	list->tail->nextNode = node;
	list->tail = node;
	list->nodeCount += 1;
}

bool Goal_test(Node* node, Node* goal)
{
	return is_equal(node->state, goal->state);
}

bool checkExist(NodeList* list, Node* node) //
{
	Node* n;
	int i = 0;
	n = list->head;
	while (i < list->nodeCount) 
	{
		if (Goal_test(node, n))
		{
			return true;
		}
		n = n->nextNode;
		i++;
	}
	return false;
}


int random(int a, int b)
{
	//srand(time(NULL));
	int i = a + rand() % (b + 1 - a);
	return i;
}
int h1cost(Node* node, Node* goal)
{
	int cost = 0;
	for (int i = 0; i < SIDE; i++)
		for (int j = 0; j < SIDE; j++)
		{
			if (node->state[i][j] != goal->state[i][j])
				cost++;
		}
	return cost;
}

void findNumber(int A[][SIDE], int& row, int& col, int x)
{
	for (int i = 0; i < SIDE; i++)
		for (int j = 0; j < SIDE; j++)
		{
			if (A[i][j] == x)
			{
				row = i;
				col = j;
				return;
			}
		}
}

int h2cost(Node* node, Node* goal)  //tổng số lần đi đến đích của các ô
{
	int h2 = 0;
	int row = 0, col = 0;
	for (int i = 0; i < SIDE; i++)
		for (int j = 0; j < SIDE; j++)
		{
			findNumber(goal->state, row, col, node->state[i][j]);
			h2 += abs(i - row) + abs(j - col);
		}
	return h2;
}

Node* LIFO_pop(NodeList* list)
{

	Node* pop = NULL;

	if (list->head == NULL)
		return NULL;
	else
	{
		if (list->head->nextNode == NULL)
		{
			pop = list->head;
			list->head = list->head->nextNode;
			list->nodeCount = 0;
			list->head = list->tail = NULL;
		}
		else
		{
			pop = list->tail;
			Node* p = list->head;
			while (p->nextNode != list->tail)
			{
				p = p->nextNode;
			}
			list->tail = p;
			list->tail->nextNode = NULL;
			list->nodeCount--;
		}
	}
	return pop;
}

Node* dequeue(NodeList* list)
{

	Node* deq = NULL;

	if (list->head == NULL)
		return NULL;
	else
	{
		if (list->head->nextNode == NULL)
		{
			deq = list->head;
			list->head = list->head->nextNode;
			list->nodeCount = 0;
			list->head = list->tail = NULL;
		}
		else
		{
			deq = list->head;
			list->head = list->head->nextNode;
			list->nodeCount--;
		}
	}
	return deq;
}

bool isLegalActionForAStar(Node* node, Actions action, int& index_i, int& index_j)
{
	for (int i = 0; i < SIDE; i++) {
		for (int j = 0; j < SIDE; j++) {
			if (node->state[i][j] == 0) {
				index_i = i;
				index_j = j;
				i = SIDE;
				break;
			}
		}
	}
	if (action == Actions::left && index_j == 0)  return false;
	if (action == Actions::right && index_j == 2)  return false;

	switch (index_i)
	{
	case 0:
		if (action == Actions::up) return false;
		return true;
	case 2:
		if (action == Actions::down) return false;
		return true;
	default:
		return true;
	}
	return true;
}

Node* Child_nodeForAStar(Node* node, Actions action, Node* Goal)
{

	Node* child = new Node();
	int index_i, index_j;
	if (isLegalActionForAStar(node, action, index_i, index_j) == true)
	{
		copy_array(child->state, node->state);
		child->action = action;
		child->nextNode = NULL;
		child->parent = node;
		child->depth = node->depth + 1;
		if (action == Actions::down)
		{
			swap(child->state[index_i][index_j], child->state[index_i + 1][index_j]);
		}
		else if (action == Actions::up)
		{
			swap(child->state[index_i][index_j], child->state[index_i - 1][index_j]);
		}
		else if (action == Actions::left)
		{
			swap(child->state[index_i][index_j], child->state[index_i][index_j - 1]);
		}
		else if (action == Actions::right)
		{
			swap(child->state[index_i][index_j], child->state[index_i][index_j + 1]);
		}
		child->path_cost = h2cost(child, Goal) + h1cost(child, Goal);
	}
	else
		return NULL;
	return child;

}

Node* FindMinValue(NodeList* list)
{
	Node* min = list->head;
	Node* p = list->head->nextNode;
	while (p != NULL)
	{
		if (p->path_cost < min->path_cost)
			min = p;
		p = p->nextNode;
	}
	if (min == list->head)
		return dequeue(list);
	else if (min == list->tail)
	{
		return LIFO_pop(list);
	}
	else
	{
		Node* prevNode = new Node();
		prevNode = list->head;
		p = list->head->nextNode;
		while (p != min)
		{
			prevNode = p;
			p = p->nextNode;
		}
		prevNode->nextNode = p->nextNode;
		p->nextNode = NULL;
		return p;

	}
}
void MoveAStar(Node* node, Node* goal)
{
	NodeList* Now = Initial();
	Node* move = Child_nodeForAStar(node, Actions::down, goal);
	if (move != NULL)
		enqueue(Now, move);

	move = Child_nodeForAStar(node, Actions::up, goal);
	if (move != NULL)
		enqueue(Now, move);

	move = Child_nodeForAStar(node, Actions::left, goal);
	if (move != NULL)
		enqueue(Now, move);

	move = Child_nodeForAStar(node, Actions::right, goal);
	if (move != NULL)
		enqueue(Now, move);
	node->nextNode = Now->head;
}

bool checkExistForAStar(NodeList* list, Node* node)
{
	if (list->head != NULL)
	{
		Node* child = list->head;
		while (child != NULL)
		{
			if (is_equal(child->state, node->state))
			{
				return true;
			}

			child = child->nextNode;
		}
	}
	return false;
}
int AStarSearch(Node* root, Node* goal)
{
	NodeList* ListA = Initial();
	NodeList* ListB = Initial();
	enqueue(ListA, root);
	bool isGoal = false;
	int i = 2000;
	while (i > 0)
	{
		Node* Cur = FindMinValue(ListA);
		enqueue(ListB, createNode(Cur));
		MoveAStar(Cur, goal);
		Node* CurChild = Cur->nextNode;
		while (CurChild != NULL)
		{

			if (Goal_test(CurChild, goal))
			{
				isGoal = true;
				return 1;
			}
			bool x = checkExistForAStar(ListA, CurChild);
			bool y = checkExistForAStar(ListB, CurChild);
			if (!x && !y)
			{
				Node* Temp = createNode(CurChild);
				Temp->nextNode = NULL;
				enqueue(ListA, Temp);
			}
			CurChild = CurChild->nextNode;
		}
		i--;
	}
	return 0;
}

int FindMin(double a, double b, double c, double d)
{
	double min1 = min(a, b);
	double min2 = min(c, d);
	if (min1 < min2)
	{
		if (a < b)
			return 0;
		else
			return 1;
	}
	else
	{
		if (c < d)
			return 2;
		else
			return 3;
	}
}

NodeList* List;

double ExpectimaxN(Node* node, Node* goal, int& action, int depth, bool isMax)
{
	if (Goal_test(node, goal) == true)
	{
		return -1000000 * (depth + 1);
	}

	if (depth == 0)
		return 1;
	//return 0;
	float k = float(D) / 2;
	if (checkExist(List, node) == true)
	{

		return 1; // khong lay gia tri
	}
	else if (depth > k + 1)
		enqueue(List, node);
	double a, b, c, d;
	int dem = 0;//de chia trung binh
	if (isMax)
	{
		if (isLegalAction(node, (Actions)0) == true)
		{
			a = ExpectimaxN(Child_node(node, (Actions)0), goal, action, depth - 1, false);
			dem++;
		}
		else
			a = 4;
		if (isLegalAction(node, (Actions)1) == true)
		{
			b = ExpectimaxN(Child_node(node, (Actions)1), goal, action, depth - 1, false);
			dem++;
		}
		else
			b = 4;

		if (isLegalAction(node, (Actions)2) == true)
		{
			c = ExpectimaxN(Child_node(node, (Actions)2), goal, action, depth - 1, false);
			dem++;
		}
		else
			c = 4;
		if (isLegalAction(node, (Actions)3) == true)
		{
			d = ExpectimaxN(Child_node(node, (Actions)3), goal, action, depth - 1, false);
			dem++;
		}
		else
			d = 4;
		action = FindMin(a, b, c, d);

		return min(min(a, b), (min(c, d)));
	}
	else
	{
		if (isLegalAction(node, (Actions)0) == true)
		{
			a = ExpectimaxN(Child_node(node, (Actions)0), goal, action, depth - 1, true);
			dem++;
		}
		else
			a = 4;
		if (isLegalAction(node, (Actions)1) == true)
		{
			b = ExpectimaxN(Child_node(node, (Actions)1), goal, action, depth - 1, true);
			dem++;
		}
		else
			b = 4;

		if (isLegalAction(node, (Actions)2) == true)
		{
			c = ExpectimaxN(Child_node(node, (Actions)2), goal, action, depth - 1, true);
			dem++;
		}
		else
			c = 4;
		if (isLegalAction(node, (Actions)3) == true)
		{
			d = ExpectimaxN(Child_node(node, (Actions)3), goal, action, depth - 1, true);
			dem++;
		}
		else
			d = 4;
		action = FindMin(a, b, c, d);

		return (a + b + c + d) / (float)dem;

	}
}


bool CheckLegalState(int A[][SIDE])
{
	int i, j;
	int m, n;
	for(i=0;i<SIDE;i++)
		for (j = 0; j < SIDE; j++)
		{
			if (A[i][j] < 0 || A[i][j]>8)
				return false;
			for(m=i;m<SIDE;m++)
				for (n = j + 1; m < SIDE; m++)
				{
					if (A[i][j] == A[m][n])
						return false;
				}
		}
	return true;
}

void play_8_puzzle(int turn)
{
	Node* Goal = new Node();
	Node* Start = new Node();
	Goal->parent = NULL;
	Goal->depth = 0;

	int i, j;

	Goal = (Node*)malloc(sizeof(Node));
	Start = (Node*)malloc(sizeof(Node));

	printf("Enter the goal state\n");
	while (1)
	{
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				std::cin >> Goal->state[i][j];
		if (CheckLegalState(Goal->state) == true)
			break;
		printf("Illegal Goal state, Please try again\n");
	}
	printf("Goal state:\n");
	
	print_array(Goal->state);

	Goal->parent = NULL;
	printf("Enter the current state\n");
	while (1)
	{
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				std::cin >> Start->state[i][j];
		if (CheckLegalState(Start->state) == true)
		{
			Start->parent = NULL;
			Start->path_cost = h2cost(Start, Goal) + h1cost(Start, Goal);
			Start->nextNode = NULL;
			if (AStarSearch(Start, Goal) == true)
			{
				break;
			}
		}
		printf("Illegal Current state, Please try again\n");
	}
	printf("Start state:\n");
	print_array(Start->state);

	int win = 1;// nếu win = 1 là người thắng, win = 0 là máy thắng
	int s = 0;
	int dx;  //biểu diễn mặt của đồng xu
	int act; // hành động của người
	int action; // hành động của máy.

	Node* ComNode;
	srand(time(NULL));
	while (1)
	{
		if (Goal_test(Start, Goal) == true)
			break;
		if (turn == 1)
		{
			printf("Press 1 to flip a coin:  ");
			std::cin >> s;
			if (s == 1)
			{
				Sleep(1000);
				dx = random(1, 2); // tung đồng xu
				if (dx == 1)
					printf("\nYou get a head - Play:\n");
				else
				{
					printf("\nYou get a tail - Computer turn:\n");
					turn = 0;
					continue;
				}
			}
			else
				continue;
			printf("\nChoise a action: 1 - left  ||  2 - right  ||  3 - up  ||  4 - down \n");
			std::cin >> act;
			while (isLegalAction(Start, (Actions)(act - 1)) == false)
			{
				printf("IllegalAction, Please choice again:\n");
				std::cin >> act;
			}
			Node* newnode = NewNode(Start, (Actions)(act - 1));
			copy_array(Start->state, newnode->state);
			delete newnode;
			print_array(Start->state);
			if (Goal_test(Start, Goal)) {
				win = 1;
				break;
			}
		}
		else if (turn == 0)
		{
			Sleep(3000);
			dx = random(1, 2); //tung đồng xu để biết lượt chơi
			printf("COMPUTER is filping a coin\n");
			Sleep(3000);
			if (dx == 1)
			{
				printf("\nCOMPUTER get a head - Computer play: \n\n");
			}
			else
			{
				printf("\nCOMPUTER get a tail - Your turn: \n\n");
				turn = 1;
				continue;
			}

			List = Initial();

			double k = ExpectimaxN(Start, Goal, action, D, true);
			std::cout << "\naction: " << action_names[action] << std::endl;
			while (1)
			{
				if (isLegalAction(Start, (Actions)action) == true)
				{
					Start = Child_node(Start, (Actions)action);
					break;
				}
				else
				{
					action = random(1, 4);
				}
			}
			print_array(Start->state);

			if (Goal_test(Start, Goal))
			{
				win = 0;
				break;
			}
			free(List);
		}
		fflush(stdin);
	}
	if (win == 1)
		printf("\n\nYOU WIN\n\n");
	else
		printf("\n\nCOMPUTER WIN\n\n");

}


int main()
{
	play_8_puzzle(0); // 0 là máy chơi trước, 1 là người chơi trước

	return 0;
}

