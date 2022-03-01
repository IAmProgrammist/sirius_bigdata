#include <iostream>
#include <string>
#include <set>
#include <string>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <map>
#include <regex>
#include <algorithm> 
#include <cctype>
#include <locale>
using namespace std;

// The code below is written with pain and suffer. 
// By VladOS (IAmProgrammist)


// Рабочее пространство
const string WORK_FOLDER_TASK_1 = "C:\\Users\\vladi\\Downloads\\task_1";

// Посимвольная проверка алфавита
const bool REQUIRED_ALPHABET_CHECK = false;

// Проверка работы алгоритма на вводе из строки консоли (ст. файл)
const bool REQUIRED_MANUAL_INPUT_QUERY = false;

// Есть возможность создания файлов для Gephi
const bool REQUIERED_GEPHI_OUTPUT = false;

// Проверка слов из файла
const bool REQUIERED_FILE_INPUT_QUERY = true;

// Сравнивает работу двух алгоритмов и выводи ошибку, если что-то не так
const bool REQUIERED_ALGO_CHECK = false;

// Путь до словаря
const string PATH_DICT_TASK1 = WORK_FOLDER_TASK_1 + "\\dict.txt";

// Путь до запросов
const string PATH_QUERIES_TASK1 = WORK_FOLDER_TASK_1 + "\\queries.txt";

// Путь до ответов
const string PATH_ANSWER_TASK1 = WORK_FOLDER_TASK_1 + "\\queriesans.txt";

// Путь до вывода Gephi (вершины)
const string PATH_GEPHI_NODES_TASK1 = WORK_FOLDER_TASK_1 + "\\nodes.csv";

// Путь до вывода Gephi (рёбра)
const string PATH_GEPHI_LINKS_TASK1 = WORK_FOLDER_TASK_1 + "\\links.csv";

//Глубина слов для Gephi
const int GEPHI_MAX_WORD_LENGTH = 99;

// id для вывода в файл для Gephi
long long idCounterCharacters = 0;


class character {
public:

	long long id;

	character* addCharacter(char ch) const {
		character* charr = new character(ch);
		linkedCharacters.insert(charr);
		return charr;
	}
	character* addCharacter(char ch, string* finalWordLink) const {
		character* charr = new character(ch, finalWordLink);
		linkedCharacters.insert(charr);
		return charr;
	}

	char ch;

	mutable string* finalWord = nullptr;
	struct CHARACTERS_COMPARATOR {
		bool operator()(const character* first, const character* second) const {
			return first->ch < second->ch;
		}
	};
	mutable set<character*, CHARACTERS_COMPARATOR> linkedCharacters;


	~character() {
		for (set<character*>::iterator i = linkedCharacters.begin(); i != linkedCharacters.end(); i++) {
			delete (*i);
		}
		delete finalWord;
	}

	character() {
		ch = -1;
		id = idCounterCharacters++;
	}


	const character* getOrDefault(char req_ch) const {
		character* charrr = new character(req_ch);
		--idCounterCharacters;
		auto foundChar = linkedCharacters.find(charrr);
		if (foundChar != linkedCharacters.end()) {
			const character* returnVal = *(foundChar);
			delete charrr;
			return returnVal;
		}
		else {
			const character* returnVal = addCharacter(req_ch);
			delete charrr;
			return returnVal;
		}
	}

	const character* getOrDefault(char req_ch, string* finWord) const {
		character* charrr = new character(req_ch);
		--idCounterCharacters;
		auto foundChar = linkedCharacters.find(charrr);
		if (foundChar != linkedCharacters.end()) {
			const character* returnVal = *(foundChar);
			returnVal->finalWord = finWord;
			delete charrr;
			return returnVal;
		}
		else {
			const character* returnVal = addCharacter(req_ch, finWord);
			delete charrr;
			return returnVal;
		}
	}

	const character* get(char req_ch) const {

		character* charrr = new character(req_ch);
		auto foundChar = linkedCharacters.find(charrr);
		if (foundChar != linkedCharacters.end()) {
			const character* returnVal = *(foundChar);
			delete charrr;

			return returnVal;
		}
		else {
			delete charrr;
			return nullptr;
		}
	}

	double getWeight() {
		if (linkedCharacters.size() == 0) return basisWeight;
		double returnWeight = basisWeight;
		for (set<character*>::iterator it = linkedCharacters.begin(); it != linkedCharacters.end(); it++) {
			returnWeight += (*it)->getWeight();
		}
		return returnWeight;
	}

private:
	character(char p_ch) {
		id = idCounterCharacters++;
		ch = p_ch;
	};

	character(char p_ch, string* fWord) {
		id = idCounterCharacters++;
		ch = p_ch;
		finalWord = fWord;
	}

	const double basisWeight = 0.1;
};




void addCharacter(const character* charct, string* str, int offset) {
	if (offset == str->size() - 1) {
		charct->getOrDefault(str->at(offset), str);
	}
	else {
		addCharacter(charct->getOrDefault(str->at(offset)), str, offset + 1);
	}
}


ofstream links;
ofstream nodes;

void writefile(const character* charr, const int MAX_WORD_LENGTH, int currentlength) {
	if (currentlength >= MAX_WORD_LENGTH) return;
	nodes << charr->id << ",\"" << charr->ch;
	if (charr->finalWord == nullptr) {
		nodes << "\"" << endl;
	}
	else {
		nodes << ", " << *charr->finalWord << "\"" << endl;
	}
	for (set<character*>::iterator i = charr->linkedCharacters.begin(); i != charr->linkedCharacters.end(); i++) {
		links << charr->id << "," << (*i)->id << "," << (*i)->getWeight() << endl;
		writefile(*i, MAX_WORD_LENGTH, currentlength + 1);
	}
}

//Алгоритм возвращает все возможные слова
pair<int, set<string*>> findWord(const character* currentChar, string currentWord, int failureCount) {
	if (currentWord.size() == 0 && currentChar->finalWord != nullptr && *currentChar->finalWord != "The Root") {
		set<string*> answer;
		answer.insert(currentChar->finalWord);
		return pair<int, set<string*>>(failureCount, answer);
	}
	const character* toGet = currentChar->get(currentWord[0]);
	pair<int, set<string*>> found(3, set<string*>());
	if (toGet != nullptr) {
		pair<int, set<string*>> tmpAns = findWord(toGet, currentWord.substr(1), failureCount);
		if (tmpAns.first < found.first) found = tmpAns;
	}
	if (found.first == 0) return found;
	if (failureCount == 0 && currentWord.size() >= 3 && found.first >= 2) {
		string oldword = currentWord;
		swap(currentWord[0], currentWord[1]);
		swap(currentWord[1], currentWord[2]);
		pair<int, set<string*>> tmpAns = findWord(currentChar, currentWord, failureCount + 2);
		if (found.first == tmpAns.first) {
			found.second.insert(tmpAns.second.begin(), tmpAns.second.end());
		}
		else if (found.first > tmpAns.first) {
			found = tmpAns;
		}
		currentWord = oldword;
	
	}
	if (failureCount != 2) {
		for (set<character*>::iterator i = currentChar->linkedCharacters.begin(); i != currentChar->linkedCharacters.end(); i++) {
			// Добавление символа перед (по сути пробрасывание того же слова):
			pair<int, set<string*>> tmpAns = findWord((*i), currentWord, failureCount + 1);
			if (found.first == tmpAns.first) {
				found.second.insert(tmpAns.second.begin(), tmpAns.second.end());
			}
			else if (found.first > tmpAns.first) {
				found = tmpAns;
			}
			// Замена символа:
			if (currentWord.size() >= 1) {
				tmpAns = findWord((*i), currentWord.substr(1), failureCount + 1);
				if (found.first == tmpAns.first) {
					found.second.insert(tmpAns.second.begin(), tmpAns.second.end());
				}
				else if (found.first > tmpAns.first) {
					found = tmpAns;
				}
			}
		}
		// Удаление следующего символа (выглядит плохо, но должно сработать):
		if (currentWord.size() >= 1) {
			pair<int, set<string*>> tmpAns = findWord(currentChar, currentWord.substr(1), failureCount + 1);
			if (found.first == tmpAns.first) {
				found.second.insert(tmpAns.second.begin(), tmpAns.second.end());
			}
			else if (found.first > tmpAns.first) {
				found = tmpAns;
			}
		}
		// Меняем символы местами (эм хз лол):
		if (currentWord.size() >= 2) {
			swap(currentWord[0], currentWord[1]);
			pair<int, set<string*>> tmpAns = findWord(currentChar, currentWord, failureCount + 1);
			if (found.first == tmpAns.first) {
				found.second.insert(tmpAns.second.begin(), tmpAns.second.end());
			}
			else if (found.first > tmpAns.first) {
				found = tmpAns;
			}
		}
	}
	return found;
}

//Алгоритм возвращает исправления
pair<int, vector<string>> findWord(const character* currentChar, string originalWord, int at, int failureCount) {
	if (originalWord.size() - at <= 0 && currentChar->finalWord != nullptr && !originalWord.empty()) {
		vector<string> ans;
		ans.push_back(*currentChar->finalWord);
		return pair<int, vector<string>>(failureCount, ans);
	}
	pair<int, vector<string>> found;
	found.first = 3;
	if (originalWord.size() - at > 0) {
		const character* toGet = currentChar->get(originalWord[at]);
		if (toGet != nullptr) {
			pair<int, vector<string>> tmpAns = findWord(toGet, originalWord, at + 1, failureCount);

			if (found.first > tmpAns.first) {
				found = tmpAns;
			}
		}
	}

	if (found.first == 0) return found;

	if (failureCount == 0 && originalWord.size() - at >= 3 && found.first == 3) {
		string oldoriginalword = originalWord;
		pair<int, vector<string>> foundOld = found;
		swap(originalWord[at], originalWord[at + 1]);
		found.second.push_back(originalWord);
		swap(originalWord[at + 1], originalWord[at + 2]);
		pair<int, vector<string>> tmpAns = findWord(currentChar, originalWord, at, failureCount + 2);
		if (tmpAns.first != 3) {
			found.second.push_back(tmpAns.second[0]);
			found.first = 2;
		}
		else {
			found = foundOld;
		}
		originalWord = oldoriginalword;

		//Отдельный случай, swap и insert.
		string changedWord = originalWord;
		vector<string> changes;

	}
	if (failureCount < 2) {
		for (set<character*>::iterator i = currentChar->linkedCharacters.begin(); i != currentChar->linkedCharacters.end(); i++) {
			string changedWord = originalWord;
			changedWord.insert(at, 1, (*i)->ch);
			pair<int, vector<string>> tmpAns = findWord((*i), changedWord, at + 1, failureCount + 1);
			if (tmpAns.first < found.first) {
				if (tmpAns.first - (failureCount + 1) == 0) {
					found.first = tmpAns.first;
					found.second = vector<string>();
					found.second.push_back(changedWord);
				}
				else if (tmpAns.first - (failureCount + 1) == 1) {
					found = tmpAns;
					found.second.insert(found.second.begin(), changedWord);
				}
			}
			changedWord = originalWord;

			if (originalWord.size() - at > 0) {
				changedWord[at] = (*i)->ch;
				tmpAns = findWord((*i), changedWord, at + 1, failureCount + 1);
				if (tmpAns.first < found.first) {
					if (tmpAns.first - (failureCount + 1) == 0) {
						found.first = tmpAns.first;
						found.second = vector<string>();
						found.second.push_back(changedWord);
					}
					else if (tmpAns.first - (failureCount + 1) == 1) {
						found = tmpAns;
						found.second.insert(found.second.begin(), changedWord);
					}
				}
				changedWord = originalWord;
			}
		}
		if (originalWord.size() - at > 0) {
			string changedWord = originalWord;
			changedWord.erase(at, 1);
			pair<int, vector<string>> tmpAns = findWord(currentChar, changedWord, at, failureCount + 1);
			if (tmpAns.first < found.first) {
				if (tmpAns.first - (failureCount + 1) == 0) {
					found.first = tmpAns.first;
					found.second = vector<string>();
					found.second.push_back(changedWord);
				}
				else if (tmpAns.first - (failureCount + 1) == 1) {
					found = tmpAns;
					found.second.insert(found.second.begin(), changedWord);
				}
			}
			changedWord = originalWord;
		}
		if (originalWord.size() - at > 0) {
			string changedWord = originalWord;
			swap(changedWord[at], changedWord[at + 1]);
			pair<int, vector<string>> tmpAns = findWord(currentChar, changedWord, at, failureCount + 1);
			if (tmpAns.first < found.first) {
				if (tmpAns.first - (failureCount + 1) == 0) {
					found.first = tmpAns.first;
					found.second = vector<string>();
					found.second.push_back(changedWord);
				}
				else if (tmpAns.first - (failureCount + 1) == 1) {
					found = tmpAns;
					found.second.insert(found.second.begin(), changedWord);
				}
			}
			changedWord = originalWord;
		}
	}
	return found;
}

void firstTask() {
	const character* root = new character();
	string* rootFWord = new string("The Root");
	root->finalWord = rootFWord;
	ifstream input;

	input.open(PATH_DICT_TASK1);
	int i = 0;
	while (!input.eof())
	{

		i++; string inpText;
		getline(input, inpText);
		if (inpText == "") continue;
		string* inpWordK = new string(inpText.begin(), inpText.end());
		addCharacter(root, inpWordK, 0);
	}
	const character* findRoot = root;
	if (REQUIRED_ALPHABET_CHECK) {
		while (true)
		{
			cout << "Available characters: " << endl;
			for (set<character*>::iterator i = findRoot->linkedCharacters.begin(); i != findRoot->linkedCharacters.end(); i++) {
				cout << (*i)->ch << " ";
			}
			string inp;
			cout << endl;
			getline(cin, inp);
			char c = inp[0];
			findRoot = findRoot->getOrDefault(c);
			if (findRoot->finalWord != nullptr) {
				cout << "You achieved final word! " << *(findRoot->finalWord) << endl;
			}
		}
	}
	if (REQUIERED_FILE_INPUT_QUERY) {
		ifstream queries;
		ofstream queriesans;
		queries.open(PATH_QUERIES_TASK1);
		queriesans.open(PATH_ANSWER_TASK1);
		int i = 0;
		while (!queries.eof())
		{
			i++;
			if (i % 10 == 0) {
				cout << "Progress... " << static_cast<double>(i) / 1000.0 << "% is done." << endl;
			}
			string userinput;
			getline(queries, userinput);
			if (userinput != "") {
				pair<int, vector<string>> answer = findWord(root, userinput, 0, 0);
				if (REQUIERED_ALGO_CHECK) {
					pair<int, set<string*>> answer2 = findWord(root, userinput, 0);
					if (answer2.first != answer.first) {
						cout << "Alert! The second algo is better! " << userinput << " " << i << endl;

						cout << "First algo output: " << answer2.first << " ";

						for (set<string*>::iterator i = answer2.second.begin(); i != answer2.second.end(); i++) {
							cout << *(*i) << " ";
						}

						cout << endl;

						cout << "Second algo output: " << answer.first << " ";
						for (int i = 0; i < answer.second.size(); i++) {
							cout << answer.second[i] << " ";
						}
						cout << endl << endl;
						return;
					}
				}
				if (answer.first == 0) {
					queriesans << userinput << " 0" << endl;
				}
				else if (answer.first == 1) {
					queriesans << userinput << " 1 " << answer.second[0] << endl;
				}
				else if (answer.first == 2) {
					queriesans << userinput << " 2 " << answer.second[0] << " " << answer.second[1] << endl;
				}
				else {
					queriesans << userinput << " 3+" << endl;
				}
			}
		}
		queriesans.flush();
		queriesans.close();
	}
	if (REQUIRED_MANUAL_INPUT_QUERY) {
		while (1) {
			string userinput;
			getline(cin, userinput);
			if (userinput != "") {
				pair<int, vector<string>> answer = findWord(root, userinput, 0, 0);
				if (answer.first == 0) {
					cout << userinput << " 0" << endl;
				}
				else if (answer.first == 1) {
					cout << userinput << " 1 " << answer.second[0];
				}
				else if (answer.first == 2) {
					cout << userinput << " 2 " << answer.second[0] << " " << answer.second[1] << endl;
				}
				else {
					cout << userinput << " 3+" << endl;
				}
			}
		}
	}
	if (REQUIERED_GEPHI_OUTPUT) {
		nodes.open(PATH_GEPHI_NODES_TASK1);
		links.open(PATH_GEPHI_LINKS_TASK1);
		nodes << "id,label" << endl;
		links << "source,target,weight" << endl;
		writefile(root, GEPHI_MAX_WORD_LENGTH, 0);
		nodes.flush();
		nodes.close();
		links.flush();
		links.close();
	}
	delete root;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	firstTask();
	return 0;
}