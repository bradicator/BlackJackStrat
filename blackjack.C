/*
 * blackjack.C
 *
 *  Created on: Apr 16, 2020
 *      Author: ruofeizhao
 */
#include "blackjack.H"

int Deck::deal() {
	for (auto cp=cards.begin(); cp != cards.end(); cp++) {
		if (*cp > 0) {
			int dealtcard = *cp;
			deadcards.push_back(dealtcard);
			*cp = 0;
			return dealtcard;
		}
	}
	return -1;
}

Deck& Deck::removeCard(int card){
	auto p = find(cards.begin(), cards.end(), card);
	if (p != cards.end()) {deadcards.push_back(*p); *p = 0;}
	return *this;
}

double Deck::getCardProb(int card) {
	int count = count_if(cards.begin(), cards.end(), [card](int i){return i==card;});
	if (count == 0) return -1;
	else return (double)count / getNLive();
}

void Deck::reset() {
	cards = vector<int>{1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,8,8,8,
		8,9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
	deadcards = vector<int>{};
}

void Hand::update() {
	hard = 0;
	for (auto&& c : hand) {
		if (c == 1) naces++;
		hard += c;
	}
	hard_eff = (hard <= 21) ? hard : -1;
	soft_eff = hard + naces*10;
	while (soft_eff > 21 && soft_eff > hard) soft_eff -= 10;
	if (soft_eff > 21) soft_eff = -1;
	eff = max(soft_eff, hard_eff);
}

ProbTable getDealerProbTable(int dcard, initializer_list<int> blockers, unsigned N) {
	ProbTable result {};
	result[-1] = 0;result[17] = 0;result[18] = 0;result[19] = 0;result[20] = 0;result[21] = 0;
	Hand temphand = Hand(blockers); if (temphand.getEff() == -1) return result;
	vector<int> blocker_all {dcard};
	blocker_all.insert(blocker_all.end(), blockers.begin(), blockers.end());
	mt19937 g(N);
	int count = 0;
	Deck rawdeck {blocker_all};
	Hand rawhand {dcard};
	for (unsigned i=0; i<N; i++) {
		Deck deck(rawdeck);
		Hand hand(rawhand);
		deck.shuffle(g);
		//for (auto i : deck.getDeadCards()) cout << i;
		//cout << endl;
		while (!hand.isDealerDone()) hand.hit(deck.deal());
		//for (auto i : hand.getHand()) cout << i << " ";
		//cout << endl;
		//cout << endl;
		if (!hand.isNBJ()){result[hand.getEff()] += 1; count++;}
	}
	for (auto&& item : result) item.second /= count;
	return result;
}

map<int, ProbTable> readDealerProbTable(string path) {
	ifstream infile(path);
	ProbTable result {};
	map<int, ProbTable> lookupdict {};
	int key;
	double pb, p17, p18, p19, p20, p21;
	while (infile >> key >> pb >> p17 >> p18 >> p19 >> p20 >> p21) {
		result[-1] = pb;
		result[17] = p17;result[18] = p18;result[19] = p19;result[20] = p20;result[21] = p21;
		lookupdict[key] = result;
	}
	infile.close();
	return lookupdict;
}

int getBlockerKey(int d, vector<int> hvec) {
	// d, smaller, larger...
	int key = 0;
	sort(hvec.begin(), hvec.end());
	int digit {1};
	int len = (hvec.size() > 4) ? 4 : hvec.size();
	for (int idx=0; idx<len; idx++) {
		key += hvec[idx] * digit;
		digit *= 100;
	}
	key += d * digit;
	return key;
}

void genDealerProbTable(unsigned N, string path) {
	int key = 0;
	ofstream myfile(path);
	for (int i=1; i!=11; i++) {
		for (int j=1; j!=11; j++) {
			for (int k=j; k!=11; k++) {
				for (int l=k; l!=11; l++) {
					for (int m=l; m!=11; m++) {

						auto result = getDealerProbTable(i, {j, k, l, m}, N);
						key =  getBlockerKey(i, vector<int>{j, k, l, m});
						cout << key << endl;
						myfile << key << " ";
						myfile << result[-1] << " ";
						myfile << result[17] << " ";
						myfile << result[18] << " ";
						myfile << result[19] << " ";
						myfile << result[20] << " ";
						myfile << result[21] << endl;
					}
				}
			}
		}
	}

	for (int i=1; i!=11; i++) {
		for (int j=1; j!=11; j++) {
			for (int k=j; k!=11; k++) {
				for (int l=k; l!=11; l++) {
					auto result = getDealerProbTable(i, {j, k, l}, N);
					key =  getBlockerKey(i, vector<int>{j, k, l});
					cout << key << endl;
					myfile << key << " ";
					myfile << result[-1] << " ";
					myfile << result[17] << " ";
					myfile << result[18] << " ";
					myfile << result[19] << " ";
					myfile << result[20] << " ";
					myfile << result[21] << endl;
				}
			}
		}
	}

	for (int i=1; i!=11; i++) {
		for (int j=1; j!=11; j++) {
			for (int k=j; k!=11; k++) {
				auto result = getDealerProbTable(i, {j, k}, N);
				key =  getBlockerKey(i, vector<int>{j, k});
				cout << key << endl;
				myfile << key << " ";
				myfile << result[-1] << " ";
				myfile << result[17] << " ";
				myfile << result[18] << " ";
				myfile << result[19] << " ";
				myfile << result[20] << " ";
				myfile << result[21] << endl;
			}
		}
	}
	myfile.close();
	return;
}


double getH2DReturn(Hand& hand, ProbTable& dtable) {
	// get unscaled return
	int eff = hand.getEff();
	if (eff == -1) return -100;
	double winprob {};
	double loseprob {};
	for (auto&& item : dtable) {
		if (item.first > eff) loseprob += item.second;
		else if (item.first < eff) winprob += item.second;
	}
	return 100*winprob - 100*loseprob;
}

double getH2HReturn(Hand& phand, Hand& dhand) {
	if (phand.getEff() == -1) return -100;
	if (phand.getEff() == dhand.getEff()) return 0;
	if (phand.getEff() > dhand.getEff()) return 100;
	if (phand.getEff() < dhand.getEff()) return -100;
}

ReturnCalc::ReturnCalc(Hand hand, int dc, map<int, ProbTable>& lookuptable) {
	inithand = hand;
	initdc = dc;
	initdeck = Deck(hand.getHand());
	initdeck.removeCard(dc);
	lkt_ptr = &lookuptable;
}

double ReturnCalc::getHitReturn(Hand hand, Deck deck) {
	if (hand.getEff() == 21 || hand.getEff() == -1) return -100;
	double hitreturn {};
	for (int c=1; c<=10; c++) {
		double prob = deck.getCardProb(c);
		if (prob > 0) {
			Hand temphand(hand);
			Deck tempdeck(deck);
			hitreturn += prob * getBestHSDReturn(temphand.hit(c), tempdeck.removeCard(c));
		}
	}
	//cout << "handeff" << hand.getEff() << "hitreturn" << hitreturn << endl;
	return hitreturn;
}

double ReturnCalc::getStandReturn(Hand hand) {
	ProbTable dtable = (*lkt_ptr)[getBlockerKey(initdc, hand.getHand())];
	return getH2DReturn(hand, dtable);
}

double ReturnCalc::getDoubleReturn(Hand hand, Deck deck) {
	if (hand.getEff() == 21 || hand.getEff() == -1) return -200;
	double dblreturn {};
	for (int c=1; c<=10; c++) {
		double prob = deck.getCardProb(c);
		if (prob > 0) {
			Hand temphand(hand);
			temphand.hit(c);
			ProbTable dtable = (*lkt_ptr)[getBlockerKey(initdc, temphand.getHand())];
			dblreturn += prob * getH2DReturn(temphand, dtable);
		}
	}
	return 2*dblreturn;
}

vector<double> ReturnCalc::getBestHSDImpl(Hand hand, Deck deck) {
	double standreturn, doublereturn, hitreturn;
	standreturn = doublereturn = hitreturn = -999;
	standreturn = getStandReturn(hand);
	if (hand.doubleable()) doublereturn = getDoubleReturn(hand, deck);
	hitreturn = getHitReturn(hand, deck);
	return vector<double>{hitreturn, standreturn, doublereturn};
}

double ReturnCalc::getBestHSDReturn(Hand hand, Deck deck) {
	auto v = getBestHSDImpl(hand, deck);
	return *max_element(v.begin(), v.end());
}

Action ReturnCalc::getBestHSDAction(Hand hand, Deck deck) {
	auto v = getBestHSDImpl(hand, deck);
	double maxreturn = *max_element(v.begin(), v.end());
	if (v[1]+PEPS > maxreturn) return STAND;
	else if (v[0]+PEPS > maxreturn) return HIT;
	else return DOUBLE;
}

vector<double> ReturnCalc::getBestHSDSImpl(Hand hand, Deck deck) {
	double standreturn, splitreturn, doublereturn, hitreturn;
	standreturn = splitreturn = doublereturn = hitreturn = -999;

	standreturn = getStandReturn(hand);
	hitreturn = getHitReturn(hand, deck);
	if (hand.doubleable()) doublereturn = getDoubleReturn(hand, deck);
	if (inithand.splitable()) splitreturn = (inithand[0] == 1) ?  getSplitAcesReturn() : getSplitReturn();
	return vector<double>{hitreturn, standreturn, doublereturn, splitreturn};
}

Action ReturnCalc::getBestHSDSAction(Hand hand, Deck deck) {
	auto v = getBestHSDSImpl(hand, deck);
	//for (auto&& i: v) cout << i << " ";
	double maxreturn = *max_element(v.begin(), v.end());
	if (v[1]+PEPS > maxreturn) return STAND;
	else if (v[0]+PEPS > maxreturn) return HIT;
	else if (v[2]+PEPS > maxreturn) return DOUBLE;
	else return SPLIT;
}

double ReturnCalc::getBestHSDSReturn(Hand hand, Deck deck) {
	auto v = getBestHSDSImpl(hand, deck);
	return *max_element(v.begin(), v.end());
}

double ReturnCalc::getDealerNBJProb() {
	if (initdc == 10 && inithand.size() == 2) return initdeck.getCardProb(1);
	if (initdc == 1 && inithand.size() == 2) return initdeck.getCardProb(10);
	return 0.0;
}

double ReturnCalc::getBestReturn() {
	double dnbjprob = getDealerNBJProb();
	if (inithand.isNBJ()) return 150*(1-dnbjprob);
	double normalreturn = getBestHSDSReturn(inithand, initdeck);
	return normalreturn - dnbjprob * (normalreturn + 100);
}

void RoundSimulator::printHand(Hand& h, string s) {
	/*
	cout << s;
	for (auto&& v : h.getHand()) cout << " " << v;
	cout << endl;
	*/

}

double RoundSimulator::getReturn() {
	if (phand.isNBJ()) { return (dhand.isNBJ()) ? 0 : 150;}
	if (dhand.isNBJ()) { return -100;}
	while (!dhand.isDealerDone()) dhand.hit(deck.deal());
	printHand(dhand, "dealer: ");
	Action firstmove = rc.getBestHSDSAction(phand, initdeck);
	switch (firstmove) {
	case DOUBLE: {
		phand.hit(deck.deal());
		printHand(phand, "phand");
		return 2*getH2HReturn(phand, dhand);
		break;
	}

	case STAND: {
		printHand(phand, "phand");
		return getH2HReturn(phand, dhand);
		break;
	}

	case HIT: {
		while (true) {
			phand.hit(deck.deal());
			Action nextaction = rc.getBestHSDAction(phand, deck);
			if (nextaction == STAND) {printHand(phand, "phand"); return getH2HReturn(phand, dhand);}
		}
		break;
	}

	case SPLIT: {
		printHand(phand, "hand to split");
		return (phand[0] == 1) ? getSplitAcesReturn() : getSplitReturn();
		break;
	}
	}
	return -10000000;
}

double RoundSimulator::getSplitAcesReturn() {
	double totalreturn {};
	Hand psplit1({phand[0]});
	Hand psplit2({phand[1]});
	psplit1.hit(deck.deal());
	psplit2.hit(deck.deal());
	totalreturn += getH2HReturn(psplit1, dhand) + getH2HReturn(psplit2, dhand);
	return totalreturn;
}

double RoundSimulator::getSplitReturn() {
	double totalreturn {};
	Hand psplit1({phand[0]});
	Hand psplit2({phand[1]});
	psplit1.hit(deck.deal());
	psplit2.hit(deck.deal());

	Action nextaction = rc.getBestHSDAction(psplit1, deck);
	switch (nextaction) {
	case STAND:
		totalreturn += getH2HReturn(psplit1, dhand);
		printHand(psplit1, "psplit1:");
		break;
	case DOUBLE:
		psplit1.hit(deck.deal());
		totalreturn += 2*getH2HReturn(psplit1, dhand);
		printHand(psplit1, "psplit1:");
		break;
	case HIT:
		while (true) {
			psplit1.hit(deck.deal());
			nextaction = rc.getBestHSDAction(psplit1, deck);
			if (nextaction == STAND) {printHand(psplit1, "psplit1:"); totalreturn += getH2HReturn(psplit1, dhand); break;}
		}
		break;
	case SPLIT:
		break;
	}

	nextaction = rc.getBestHSDAction(psplit2, deck);
	switch (nextaction) {
	case STAND:
		totalreturn += getH2HReturn(psplit2, dhand);
		printHand(psplit2, "psplit2:");
		break;
	case DOUBLE:
		psplit2.hit(deck.deal());
		totalreturn += 2*getH2HReturn(psplit2, dhand);
		printHand(psplit2, "psplit2:");
		break;
	case HIT:
		while (true) {
			psplit2.hit(deck.deal());
			nextaction = rc.getBestHSDAction(psplit2, deck);
			if (nextaction == STAND) {printHand(psplit2, "psplit2:"); totalreturn += getH2HReturn(psplit2, dhand); break;}
		}
		break;
	case SPLIT:
		break;
	}

	return totalreturn;
}

