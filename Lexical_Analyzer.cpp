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
#include <numeric>

#define debug cout

using namespace std;

const int MAX_NODES = 100;

ifstream input;//�����ļ����������ķ���
ofstream output;//����ļ�����token��Ԫ�飩

set<char> weight;//��ȡ�����ķ������в��ظ����ս����nfa����Ȩֵ��,��create_NFA�����г�ʼ��
set<char> C[MAX_NODES];//CΪDFAÿһ���Ӽ���Ľ�㣬�±��0��ʼ 
vector<int> dfa_final_state;//dfa����̬

int now_dfa_state;//token����dfa����ʱ����ǰ��dfa_state

vector<string> token_result;//���token�����ս��
/*����һ���ؼ������飬sacn()���룬ʶ��token��ʱ
	������ĸ����ĸ,���ȱ���������������token�Ƚ��ж��ǲ��ǹؼ���
		���ǹؼ��֣�break���keywordtoken
		�����ǣ�����DFA�ж����ǲ��ǺϷ��ı�ʶ��
*/
const char keyword[][10] = {
	"int","float","double","char","bool" ,
	"if","else" ,"main" ,"void" ,"while" ,
	"break" ,"continue" ,"return" ,
	"cin" , "cout" 
};//cin cout main �����ǹؼ��֣������б���Ϊ�ؼ�����ʶ��

struct Edge_NFA {
	char start = '0';//startΪ��ʼ���
	char val = '0';//startΪ����Ȩֵ
	char end = '0';//endΪָ��Ľ��
};

struct NFA_node {
	set<char> node;//��㼯��
	Edge_NFA edge[100];//�߼�
	int edge_num = 0;//����
}nfa;

struct Edge_DFA {
	int start = 0;//startΪ��ʼ���
	char val = '0';//startΪ����Ȩֵ
	int end = 0;//endΪָ��Ľ��
};

struct DFA_node {
	set<int> node;
	Edge_DFA edge[100];
	int edge_num = 0;
}dfa;

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
		//�ķ��ұ�ֻ��һ���ս��ֱ�ӵ���̬����һ���ս��һ�����ս����ָ����ս�����Ǹ�״̬
		if (content[4] == ' ') nfa.edge[nfa.edge_num++].end = 'Z';
		else nfa.edge[nfa.edge_num++].end = content[4];
		//��ʼ��weight���ϣ�Ϊdfa�����еĻ��� Ȩֵ
		weight.insert(content[3]);
	}
}

//debugʱ����չʾnfa���
void show_NFA_node() {
	for (auto i = nfa.node.begin(); i != nfa.node.end(); i++) {
		debug << *i << endl;
	}
}

//debugʱ����չʾnfa�ı�
void show_NFA_edge() {
	for (int i = 0; i < nfa.edge_num; i++) {
		debug << nfa.edge[i].start << "	" << nfa.edge[i].val << "	" << nfa.edge[i].end << endl;
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
	//cout << "NFA_TO_DFA Result:" << endl;
	bool marked[MAX_NODES] = { false };
	set<char> T0, T1;

	//ע������Ҫ�ֶ������̬��T0
	T0.insert('L'); T0.insert('O'); T0.insert('I'); T0.insert('A'); T0.insert('S'); T0.insert('M');
	//T0.insert('0');//����test�ļ��еĵ�dfa��ȷ���õģ��Ա�����p52��״̬ͼ��������ȷ��

	//T1��T0��Epsilon״̬�հ�
	T1 = e_closure(T0, nfa);
	//T1��Ϊ��һ���Ӽ�����ʼ��������ظ��Ļ�ת�Ӽ��հ�
	C[0] = T1;
	//��ʼ�������Ӽ�
	int i = 0;
	while (!C[i].empty() && marked[i] == false && i < MAX_NODES) {
		marked[i] = true;
		
		//���DFA��ÿ���Ӽ�C[i]
		/*printf("��%d������Ϊ��", i);
		auto count_size1 = C[i].size();
		for (auto it = C[i].begin(); it != C[i].end(); it++) {
			debug << *it ;
			if (--count_size1) debug << " , ";
		}
		debug << endl;*/

		for (auto it = weight.begin(); it != weight.end(); it++) {//weight.size()Ϊnfa�ܵ�valֵ���� 
			int j = 0;
			if (*it != '@') {
				//�ǿյĻ�ת
				set<char> temp = e_closure(move(C[i], *it, nfa), nfa);//NΪ�����ķ����� ��GΪEdge��start  val  end 
				if (!temp.empty()) {//CΪDFAÿһ����������
					bool inC = false;
					int k = 0;
					while (!C[k].empty() && k < MAX_NODES) {
						//�ж��Ƿ����ظ��Ӽ�
						if (temp == C[k]) {
							inC = true;
							break;
						}
						k++;
					}
					if (!inC) {
						//�����ظ��Ӽ����ѻ�ת�³��ֵ��Ӽ�����C
						k = 0;
						while (!C[k].empty() && k < MAX_NODES)
						{
							k++;
						}
						C[k] = temp;
					}
					dfa.node.insert(i);
					dfa.edge[dfa.edge_num].start = i;
					dfa.edge[dfa.edge_num].val = *it;
					dfa.edge[dfa.edge_num].end = k;
					dfa.edge_num++;
					//cout << i << "	input	" << *it << "	to	" << k << endl;
					//cout << i << " " << k << " " << *it << endl;//https://csacademy.com/app/graph_editor/�������ʽ����������ͼ�õ�
				}
				j++;
			}
		}
		i++;
	}
	//�������ȷ���������̬
	/*
	�������£�
	1.C[i]��ŵ���DFA�ĵ�i���Ӽ�
	2.���α���C[i]����Ϊ��˵���Ѿ�������
	3.�ǿ��Ӽ����õ���������C��Ԫ�أ�����Z����̬��㣬��˵������Ӽ���DFA����̬
	*/
	i = 0;
	while (!C[i].empty()) {
		bool is_final_state = false;
		for (auto it = C[i].begin(); it != C[i].end(); it++) {
			if (*it == 'Z') {
				is_final_state = true;
				break;
			}
		}
		if (is_final_state) {
			dfa_final_state.push_back(i);
		}
		i++;
	}
	//���DFA��̬���
	/*cout << "Final state is : ";
	for (int a = 0; a < dfa_final_state.size(); a++) {
		cout << dfa_final_state[a];
		if (a != dfa_final_state.size() - 1) cout << " , ";
	}
	cout << endl;*/
}

//debug���ڼ��dfa���еĽ��
void show_DFA_node() {
	for (auto i = dfa.node.begin(); i != dfa.node.end(); i++) {
		debug << *i << endl;
	}
}

//debug���ڼ��dfa���еı�
void show_DFA_edge() {
	for (int i = 0; i < dfa.edge_num; i++) {
		debug << dfa.edge[i].start << "	" << dfa.edge[i].val << "	" << dfa.edge[i].end << endl;
	}
}

//��ɨ�������token�Ž�dfa�ܣ����ܲ��ܵ���̬
bool token_can_run_final(string str) {
	if (str.size() == 0) {
		for (int i = 0; i < dfa_final_state.size(); i++) {
			//������dfa�ܵ���̬
			if (now_dfa_state == dfa_final_state[i])
			{
				now_dfa_state = 0;
				return true;
			}
		}
	}
	char ch = str[0];
	string str_deletefirst;
	for (int i = 1; i < str.size(); i++) str_deletefirst += str[i];
	for (int i = 0; i < dfa.edge_num; i++) {
		if (dfa.edge[i].start == now_dfa_state && dfa.edge[i].val == ch) {
			now_dfa_state = dfa.edge[i].end;
			return token_can_run_final(str_deletefirst);
		}
	}
	now_dfa_state = 0;
	return false;
}

//����Դ����
void scan() {
	ifstream read_program("source_program.txt");
	string line;
	int line_number = 1;
	while (getline(read_program,line)) {
		int i = 0;
		while (line[i] == ' ') {
			i++; 
		}
		int flag = i;
		vector<string> token;
		for (; i < line.size(); i++) {
			string temp_token;
			if (i == line.size() - 1) {
				string s;
				s+= line[i];
				token.push_back(s);
			}
			if (line[i] == ' ') {
				for (int j = flag;j<i; j++) {
					temp_token += line[j];
				}
				token.push_back(temp_token);
				flag = i + 1;
			}
		}
		i = 0;
		//����debug�������token�鿴��
		/*for (int k = 0; k < token.size(); k++) {
			debug << token[k] << " ";
		}
		cout << endl;*/
		for (int temp1 = 0; temp1 < token.size(); temp1++) {
			int all = sizeof(keyword) / sizeof(char);
			int column = sizeof(keyword[0]) / sizeof(keyword[0][0]);
			int row = all / column;
			bool is_judged = false;
			//1�����жϹؼ���keyword
			for (int temp2 = 0; temp2 < row; temp2++) {
				if (token[temp1] == keyword[temp2]) {
					is_judged = true;
					cout << line_number << "," << token[temp1] << "," << "keyword" << endl;

					output << line_number << "," << token[temp1] << "," << "keyword" << endl;
				}
			}
			if (is_judged) continue;
			//2�����жϽ��limiter
			if (token[temp1] == "," || token[temp1] == ";" || token[temp1] == "[" || token[temp1] == "]"
				|| token[temp1] == "(" || token[temp1] == ")" || token[temp1] == "{" || token[temp1] == "}") {
				is_judged = true;
				cout << line_number << "," << token[temp1] << "," << "limiter" << endl;

				output << line_number << "," << token[temp1] << "," << "limiter" << endl;
			}
			if (is_judged) continue;
			//3�����жϲ�����operator
			if (token[temp1][0] == '+' || token[temp1][0] == '-' || token[temp1][0] == '*'
				|| token[temp1][0] == '/' || token[temp1][0] == '%' || token[temp1][0] == '^'
				|| token[temp1][0] == '=' || token[temp1][0] == '>' || token[temp1][0] == '<') {
				is_judged = true;
				if (token_can_run_final(token[temp1])) {
					cout << line_number << "," << token[temp1] << "," << "operator" << endl;

					output << line_number << "," << token[temp1] << "," << "operator" << endl;
				}
				else {
					cout << line_number << "," << token[temp1] << "," << "wrong operator" << endl;

					output << line_number << "," << token[temp1] << "," << "wrong operator" << endl;
				}
			}
			if (is_judged) continue;
			//4�����жϱ�ʶ��identifier
			string str_token = token[temp1];
			for (int temp2 = 0; temp2 < token[temp1].size(); temp2++) {
				if ((token[temp1][temp2] >= 'a' && token[temp1][temp2] <= 'z' && token[temp1][temp2] != 'i' && token[temp1][temp2] != 'e') ||
					(token[temp1][temp2] >= 'A' && token[temp1][temp2] <= 'Z' && token[temp1][temp2] != 'i' && token[temp1][temp2] != 'e')) token[temp1][temp2] = 'a';
				if (token[temp1][temp2] <= '9' && token[temp1][temp2] >= '1') token[temp1][temp2] = 'c';
			}
			//����ĸ�����»��߿�ͷ�ģ�һ���Ǳ�ʶ�����жϺϷ��Լ���
			if (token[temp1][0] == '_' || token[temp1][0] == 'a') {
				if (token_can_run_final(token[temp1])) {
					cout << line_number << "," << str_token << "," << "identifier" << endl;

					output << line_number << "," << str_token << "," << "identifier" << endl;
				}
				else {
					cout << line_number << "," << str_token << "," << "wrong identifier" << endl;

					output << line_number << "," << str_token << "," << "wrong identifier" << endl;
				}
			}
			//�����ֿ�ͷ�ģ��п����ǳ���number���п����ǷǷ���ʶ��������һ��
			//5������жϳ���number
			bool is_wrong_identifier = false;
			if (token[temp1][0] == 'c' || token[temp1][0] == '0') {
				for (int l = 0; l < token[temp1].size(); l++) {
					if (token[temp1][l] == 'a') {
						is_wrong_identifier = true;
						break;
					}
				}
				if (is_wrong_identifier) {
					cout << line_number << "," << str_token << "," << "wrong identifier" << endl;

					output << line_number << "," << str_token << "," << "wrong identifier" << endl;
					continue;
				}
				else {
					if (token_can_run_final(token[temp1])) {
						cout << line_number << "," << str_token << "," << "number" << endl;

						output << line_number << "," << str_token << "," << "number" << endl;
					}
					else {
						cout << line_number << "," << str_token << "," << "wrong number" << endl;

						output << line_number << "," << str_token << "," << "wrong number" << endl;
					}
				}
			}
		}
		line_number++;
	}
}

int main() {
	create_NFA();
	//show_NFA_node();
	//show_NFA_edge();
	NFA_to_DFA();
	//show_DFA_node();
	//show_DFA_edge();
	
	//���������ķ���token�����벿��
	/*string token;
	while (cin >> token)
	{
		debug << token_can_run_final(token) << endl;
	}*/
	output.open("token_output.txt");
	scan();
	output.close();
	return 0;
}