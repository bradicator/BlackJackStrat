#include "blackjack.H"
#include <string>

void MCReturn(int N) {
	Deck fulldeck {};
	unsigned seed;
	mt19937 rg(N);
	double total {};
	double lasttotal {};
	auto lkt = readDealerProbTable();
	for (int idx=0; idx<N; idx++) {
		if (idx % 100000 == 0) {
			seed = std::chrono::system_clock::now().time_since_epoch().count();
			rg = mt19937(seed);
		}
		fulldeck.shuffle(rg);
		RoundSimulator rs(fulldeck, lkt);
		double Return = rs.getReturn();
		total += Return;
		if (idx % 100 == 0) {
			cout << "hand No: " << idx << " last 100 return: " <<
					total - lasttotal << " Avg Return: " << total / idx << endl;
			lasttotal = total;
		}
	}
}

void TheoryReturn(int N) {
	Deck fulldeck {};
	mt19937 rg(N);
	double total {};
	auto lkt = readDealerProbTable();
	for (int idx=0; idx<N; idx++) {
		fulldeck.shuffle(rg);
		ReturnCalc rc({fulldeck[0], fulldeck[1]}, fulldeck[2], lkt);
		double Return = rc.getBestReturn();
		total += Return;
		cout << "hand No: " << idx << " Expected Return: " << Return << endl;
	}
	cout << "avg return per hand: " << total / N << endl;
}

void getNextMove(vector<int> handvec, int dcard) {
	auto lkt = readDealerProbTable();
	ReturnCalc rc(handvec, dcard, lkt);
	Deck curdeck {handvec};
	curdeck.removeCard(dcard);
	Action nextmove = rc.getBestHSDSAction(handvec, curdeck);
	switch (nextmove) {
	case SPLIT:
		cout << "Split" << endl; break;
	case HIT:
		cout << "Hit" << endl; break;
	case STAND:
		cout << "Stand" << endl; break;
	case DOUBLE:
		cout << "Double" << endl; break;
	}
}

void getPostSplitNextMove(vector<int> handvec, int dcard) {
	auto lkt = readDealerProbTable();
	ReturnCalc rc(handvec, dcard, lkt);
	Deck curdeck {handvec};
	curdeck.removeCard(dcard);
	curdeck.removeCard(handvec[0]); // assume the first card is the split-on card
	Action nextmove = rc.getBestHSDAction(handvec, curdeck);
	switch (nextmove) {
	case SPLIT:
		cout << "Split" << endl; break;
	case HIT:
		cout << "Hit" << endl; break;
	case STAND:
		cout << "Stand" << endl; break;
	case DOUBLE:
		cout << "Double" << endl; break;
	}
}

int main(int argc, char** argv) {
	char mode = *argv[1];
	switch (mode) {
	case 'm':
		MCReturn(stoi(argv[2]));
		break;
	case 't':
		TheoryReturn(stoi(argv[2]));
		break;
	case 'n': {
		int dcard = stoi(argv[2]);
		vector<int> handvec {};
		for (int idx=3; idx<argc; idx++) {
			handvec.push_back(stoi(argv[idx]));
		}
		getNextMove(handvec, dcard);
		break;
	}
	case 's': {
		int dcard = stoi(argv[2]);
		vector<int> handvec {};
		for (int idx=3; idx<argc; idx++) {
			handvec.push_back(stoi(argv[idx]));
		}
		getPostSplitNextMove(handvec, dcard);
		break;
	}
	case 'g':
		genDealerProbTable(stoi(argv[2]));
		break;
	default:
		break;
	}
	return 0;
}

/*

if (argc < 4) {cerr << "at least 3 cards!" << endl; return 1;}
	auto lkt = readDealerProbTable();
	int dcard = stoi(argv[1]);
	vector<int> handvec {};
	for (int idx=2; idx<argc; idx++) {
		handvec.push_back(stoi(argv[idx]));
	}
	ReturnCalc rc(handvec, dcard, lkt);
	rc.getBestReturn();
	return 0;


if (argc > 2) {cerr << "too many args" << endl; return 1;}
	auto lkt = readDealerProbTable();
	int N = stoi(argv[1]);
	Deck fulldeck {};
	mt19937 rg(N);
	double total {};
	for (int idx=0; idx<N; idx++) {
		fulldeck.shuffle(rg);
		ReturnCalc rc({fulldeck[0], fulldeck[1]}, fulldeck[2], lkt);
		total += rc.getBestReturn();
	}
	cout << total / N << endl;

*/
