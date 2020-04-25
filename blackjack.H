#ifndef BJH
#define BJH
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <map>
#include <initializer_list>
#include <fstream>

const double PEPS {0.000001};
class Deck;
class Hand;
using namespace std;
using ProbTable = map<int, double>;
double getH2DReturn(Hand& hand, ProbTable& dtable); //get unscaled hand to distribution return
double getH2HReturn(Hand& phand, Hand& dhand); //get unscaled hand to hand return

class Deck {
public:
	Deck() {}
	Deck(initializer_list<int> blockers) {for (auto && c : blockers) removeCard(c);}
	Deck(const vector<int>& blockers) {for (auto && c : blockers) removeCard(c);}

	Deck& removeCard(int card);
	void removeCards(const vector<int>& cards) {for (auto&& c : cards) removeCard(c);}

	Deck& shuffle(mt19937& g) {for (int i=0; i<7; i++) std::shuffle(cards.begin(), cards.end(), g); return *this;}
	int deal();
	inline vector<int> getCards() const {return cards;}
	inline vector<int> getDeadCards() const {return deadcards;}
	int getNLive() {return 52 - deadcards.size();}

	double getCardProb(int card);
	int operator[](int idx) {return cards[idx];}
	void reset();

private:
	vector<int> cards {1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
	vector<int> deadcards {};
};

class Hand {
public:
	Hand(initializer_list<int> cards) : hand(cards) {update();}
	Hand(const vector<int>& cards) : hand(cards) {update();}

	Hand& hit(int card) {hand.push_back(card);update();return *this;}
	void update();
	bool inline constexpr isNBJ() {return hand.size() == 2 && eff == 21;}
	bool inline constexpr isDealerDone() {return eff == -1 || soft_eff > 17 || hard_eff >= 17;}
	int inline constexpr getEff() {return eff;}
	vector<int> getHand() {return hand;}
	bool inline splitable() {return hand.size() == 2 && hand[0] == hand[1] && hand[0] < 10;}
	bool inline doubleable() {return hand.size() == 2;}
	int operator[](int idx) {return hand[idx];}
	int size() {return hand.size();}

private:
	vector<int> hand {};
	int hard {};
	int naces {};
	int soft_eff {};
	int hard_eff {};
	int eff {};
};

enum Action {
	STAND = 0,
	HIT = 1,
	DOUBLE = 2,
	SPLIT = 3,
};

class ReturnCalc {
public:
	ReturnCalc() {}
	ReturnCalc(Hand hand, int dc, map<int, ProbTable>& lookuptable);

	double getStandReturn(Hand hand);
	double getHitReturn(Hand hand, Deck deck);
	double getDoubleReturn(Hand hand, Deck deck);

	// only call this when sure of splittable
	double getSplitReturn() {return 2*getHitReturn({inithand.getHand()[0]}, initdeck);}
	double getSplitAcesReturn() {return getDoubleReturn({1}, initdeck);}

	double getBestHSDReturn(Hand hand, Deck deck);
	Action getBestHSDAction(Hand hand, Deck deck);
	vector<double> getBestHSDImpl(Hand hand, Deck deck);

	double getBestReturn(); // best return from standard 2,1 beginning, call only in standard beginning
	double getBestHSDSReturn(Hand hand, Deck deck); //consider split, hit, stand, double
	Action getBestHSDSAction(Hand hand, Deck deck);
	vector<double> getBestHSDSImpl(Hand hand, Deck deck);

	double getDealerNBJProb();

private:
	Hand inithand {};
	Deck initdeck {};
	int initdc {};
	map<int, ProbTable>* lkt_ptr;
};

ProbTable getDealerProbTable(int dcard, initializer_list<int> blockers = {}, unsigned N=100000);
map<int, ProbTable> readDealerProbTable(string path="dpt.txt");
int getBlockerKey(int d, int h1, int h2);
void genDealerProbTable(unsigned N=10, string path="dpt.txt");
double getOverallReturn(unsigned N=10000);

class RoundSimulator {
public:
	RoundSimulator(Deck d, map<int, ProbTable>& lookuptable) : deck(d) {
		phand.hit(deck.deal()); phand.hit(deck.deal());
		dhand.hit(deck.deal()); initdeck = Deck(deck);
		dhand.hit(deck.deal());
		rc = ReturnCalc(phand, dhand[0], lookuptable);
	}

	void printHand(Hand& h, string s = "");
	double getReturn();
	double getSplitReturn();
	double getSplitAcesReturn();

private:
	Hand phand {};
	Hand dhand {};
	Deck deck {}; //shuffled
	Deck initdeck {};
	ReturnCalc rc {};
};



#endif
