#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <cstdio>
#include <stack>

using namespace std;
#define debug cout
const int MAX_NODES = 100;
ifstream input;//�����ļ����������ķ���Դ����
ofstream output;//����ļ�����token��Ԫ�飩

set<char> weight;//��ȡ�����ķ��������ս����nfa����Ȩֵ��,��create_NFA�����г�ʼ��

/*����һ���ؼ������飬sacn()���룬ʶ��token��ʱ
	������ĸ����ĸ,���ȱ���������������token�Ƚ��ж��ǲ��ǹؼ���
		���ǹؼ��֣�break���keywordtoken
		�����ǣ�����DFA�ж����ǲ��ǺϷ��ı�ʶ��
*/
const char keyword[][10] = { 
	"int","float","double","char","bool" ,
	"if","else" ,"main" ,"void" ,"while" ,
	"break" ,"continue" ,"return" ,
};

struct Edge {
	char start='0';//startΪ��ʼ���
	char val='0';//startΪ����Ȩֵ
	char end='0';//endΪָ��Ľ��
};

struct NFA_node {
	set<char> node;//��㼯��
	Edge edge[100];//�߼�
	int edge_num = 0;//����
}nfa;

//��ʼ��NFA��������̬���Z
void init_NFA() {
	nfa.node.insert('Z');
}

//��ȡ3���ķ�������һ��NFA
void create_NFA() {
	init_NFA();
	ifstream read("grammar3.txt");
	string content;
	while (getline(read, content)) {
		//�������ķ����ļ�#��ʾ������ע�ͣ�����������#endread��ʾ�ļ��Ѷ������
		if (content[0] == '#') {
			if (content == "#endread") break;
			else continue;
		}
		//debug << content << endl;
		bool isexist = false;
		//ʹ�õ��������б�����c++����ʵ�ֺܸ��ӣ���auto��Ϊ����ָ��
		//������auto����Ϊ����set�����������ͺܳ�(set<char>::iterator it;)
		for (auto i = nfa.node.begin(); i != nfa.node.end(); i++)  
		{
			if (content[0] == *i) {
				//�ķ����ķ��ս���Ѿ���nfa.node��������
				isexist = true;
				break;
			}
		}
		if (!isexist) {
			//�ķ����ķ��ս��û��nfa.node�������棬����뼯��
			nfa.node.insert(content[0]);
		}
		//�������������ķ��Ŀ�ʼ����ڲ��ڼ��ϣ���Ҫ����������
		nfa.edge[nfa.edge_num].start = content[0];
		nfa.edge[nfa.edge_num].val = content[3];
		weight.insert(content[3]);
		if (content[4] == ' ') nfa.edge[nfa.edge_num++].end = 'Z';
		else nfa.edge[nfa.edge_num++].end = content[4];
	}
}

//debugʱ����չʾnfa�ı�
void show_NFA_edge() {
	for (int i = 0; i < nfa.edge_num; i++) {
		debug << nfa.edge[i].start << "	" << nfa.edge[i].val << "	" << nfa.edge[i].end << endl;
	}
}

//debugʱ����չʾnfa���
void show_NFA_node() {
	for (auto i = nfa.node.begin(); i != nfa.node.end(); i++) {
		debug << *i << endl;
	}
}

//�������뺯��������dfa��ת�հ���ʱ��״̬���뼯�ϵĲ���
set<char> move(set<char> I, char a, NFA_node N) {
	set<char> temp;
	for (auto it = I.begin(); it != I.end(); it++)
		for (int i = 0; i < N.edge_num; i++) {
			if (N.edge[i].start == *it && N.edge[i].val == a)
				temp.insert(N.edge[i].end);
		}
	return temp;
}

//��Epsilon״̬�հ��ĺ���
set<char> e_closure(set<char> I, NFA_node N) {
	set<char> temp = I;
	stack<char> st;
	for (auto it = temp.begin(); it != temp.end(); it++)
	{
		st.push(*it);
	}
	char t;
	while (!st.empty()) {
		t = st.top();
		st.pop();
		for (int i = 0; i < N.edge_num; i++) {
			if (N.edge[i].start == t && N.edge[i].val == '@') {
				temp.insert(N.edge[i].end);
				st.push(N.edge[i].end);
			}
		}
	}
	return temp;
}

void NFA_to_DFA() {
	cout << endl << "ȷ�����DFA:" << endl;
	bool marked[MAX_NODES] = { false };
	//for (int i = 0; i < MAX_NODES; i++) marked[i] = false;
	set<char> C[MAX_NODES];//CΪDFAÿһ���������� 
	/*char s0 = nfa.edge[0].start;*/
	set<char> T0, T1;
	T0.insert('L'); T0.insert('O'); T0.insert('I'); T0.insert('A'); T0.insert('S'); T0.insert('M');

	T1 = e_closure(T0, nfa);
	C[0] = T1;
	int i = 0;
	while (!C[i].empty() && marked[i] == false && i < MAX_NODES) {
		marked[i] = true;
		//���汻ע�ʹ�����������ͼ��������ļ���

		set<char>::iterator it;
		//cout << i << ":";
		printf("��%d������Ϊ��", i);
		for (it = C[i].begin(); it != C[i].end(); it++)
			cout << *it << ",";
		cout << endl;

		for (auto it=weight.begin(); it != weight.end(); it++) {//weight.size()Ϊnfa�ܵ�valֵ���� 
			int j = 0;
			set<char> temp = e_closure(move(C[i], *it, nfa), nfa);//NΪ�����ķ����� ��GΪEdge��start  val  end 
			if (!temp.empty()) {//CΪDFAÿһ����������
				bool inC = false;
				int k = 0;
				while (!C[k].empty() && k < MAX_NODES) {
					if (temp == C[k]) {
						inC = true;
						break;
					}
					k++;
				}
				if (!inC) {
					k = 0;
					while (!C[k].empty() && k < MAX_NODES)
					{
						k++;
					}
					C[k] = temp;
				}
				cout << i << "	input	" << *it << "	to	" << k << endl;
			}
			j++;
		}
		i++;
	}
	//�������ȷ���������̬
	cout << "��̬Ϊ:";
	i = 0;
	while (!C[i].empty()) {
		bool is_final_state = false;
		for (auto it = C[i].begin(); it != C[i].end(); it++) {
			if (*it == 'Z') {
				is_final_state = true;

				break;
			}
		}
		if (is_final_state) cout << i << ",";
		i++;
	}
	cout << endl;
}

int main() {
	create_NFA();
	//show_NFA_edge();
	//show_NFA_node();
	NFA_to_DFA();
	return 0;
}