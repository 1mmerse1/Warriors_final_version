#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;
int listred[5] = {2,3,4,1,0};
int listblue[5] = {3,0,1,2,4};
int healthlist[5] = {};
int powerlist[5] = {};
string namelist[5] = {"dragon", "ninja", "iceman", "lion", "wolf"};
string teamname[2] = {"red", "blue"};
int sequence, citynum;
int k; // lion每次下降值
int hour, minute;

class weapon;
class city;
class soldier{
protected:    
    int health, place, mypower;
public:
    string name;
    int team, id;
    vector <weapon*> we;

    city * mycity{};
    soldier(int number): name(namelist[number]),health(healthlist[number]),mypower(powerlist[number]), we(10, nullptr){}
    void initplace() ;
    virtual ~soldier(); 
    virtual void setweapon() {};
    void deletemyweapon(weapon * w) {
        auto it = std::find(we.begin(), we.end(), w);
        if (it != we.end()) {
            we.erase(it);
        }
    }
    void deleteweapon(weapon * w);
    int gethealth(){
        return health;
    }
    int getmypower(){
        return mypower;
    }

    virtual void hit(soldier * s, weapon * w);
    void reducehealth(int power){
        health -= power;
    }
    bool isalive(){
        if (health <= 0) return false;
        else return true;
    }
    virtual bool isrunaway() {return false;}
    virtual void move();
    virtual void printinfo() {}
    vector <weapon*> canrob(soldier & robber);
    virtual void robweapon(soldier & s) {};
    virtual void yell() {}
    void printmove();
    void printweapon();
    int getWeaponCount() {
        int count = 0;
        for (int i = 0; i < we.size(); i++) {
            if (we[i] != nullptr) count++;
        }
        return count;
    }
    void loseWeapon(weapon* w) {
        for (int i = 0; i < we.size(); i++) {
            if (we[i] == w) {
                we[i] = nullptr;
                break;
            }
        }
    }
};
void printtime(){
    cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " ";
}
class weapon{
    public:
    int damage, wid, use = 0;
    string name;
    soldier * holder{};
    weapon (){}
    weapon(soldier *s): holder(s) {}
    void setholder(soldier & s){
        holder = &s;

    }
    virtual void beused() {}
    void setdamage() {
        if (wid == 0) damage = holder->getmypower() * 2 / 10;
        if (wid == 1) damage = holder->getmypower() * 4 / 10;
        if (wid == 2) damage = holder->getmypower() * 3 / 10;
    }
};
class arrow: public weapon{
    public:
    arrow() {name = "arrow"; wid = 2;}
    arrow(soldier *s): weapon(s) {name = "arrow"; damage = s->getmypower() * 3 / 10; wid = 2;}
    virtual void beused() {
        use++;
        if (use == 2) holder->deleteweapon(this);
    }
};
class bomb: public weapon{
    public:
    bomb() {name = "bomb"; wid = 1;}
    bomb(soldier *s): weapon(s) {name = "bomb"; damage = s->getmypower() * 4 / 10; wid = 1;}
    virtual void beused() {
        holder->deleteweapon(this);
    }
};
class sword: public weapon{

    public:
    sword() {name = "sword"; wid = 0;}
    sword(soldier *s): weapon(s) {name = "sword"; damage = s->getmypower() * 2 / 10; wid = 0;}
};
bool cmp(weapon * a, weapon * b) {
    if (a == nullptr && b == nullptr) return false;
    if (a == nullptr) return false;
    if (b == nullptr) return true;

    else if(a->wid != b->wid){
        return a->wid < b->wid;
    }
    else{
        return a->use > b->use;
    }
}
bool cmp2(weapon * a, weapon * b) {
    if (a == nullptr && b == nullptr) return false;
    if (a == nullptr) return false;
    if (b == nullptr) return true;

    else if(a->wid != b->wid){
        return a->wid < b->wid;
    }
    else{
        return a->use < b->use;
    }
}
void soldier::hit(soldier * s, weapon * w){
    s->reducehealth(w->damage);
    if (w->wid == 1) this->reducehealth(w->damage / 2);
}

class dragon: public soldier{    
    double morale;
    public:

    dragon (int sid, int life): soldier(0) {
        morale = double(life) / double (healthlist[0]);
        id = sid;
    }
    virtual void setweapon(){
        int wid = id % 3;
        weapon * temp;
        if (wid == 0)  {temp = new sword(this); we.push_back(temp);}
        if (wid == 1)  {temp = new bomb(this); we.push_back(temp);}
        if (wid == 2)  {temp = new arrow(this); we.push_back(temp);} 
        temp->setholder(*this);       
    }
    virtual void printinfo() {
        if(we[0] != nullptr) {
            cout << "It has a " << we[0]->name << ",and it's morale is " 
                 << fixed << setprecision(2) << morale << endl;
        }
    }
    virtual void yell() {
        cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " ";
        cout << teamname[team] << " dragon " << id << " yelled in city " << place << endl;
    }
};
class ninja: public soldier{
    public:
    ninja (int sid) : soldier(1){
        id = sid;
    }
    virtual void setweapon(){
        int wid = id % 3, wid2 = (id + 1) % 3;
        weapon * temp{};
        if (wid == 0)  {temp = new sword(this); we.push_back(temp);}
        if (wid == 1)  {temp = new bomb(this); we.push_back(temp);}
        if (wid == 2)  {temp = new arrow(this); we.push_back(temp);} 
        temp->setholder(*this);
        if (wid2 == 0)  {temp = new sword(this); we.push_back(temp);}
        if (wid2 == 1)  {temp = new bomb(this); we.push_back(temp);}
        if (wid2 == 2)  {temp = new arrow(this); we.push_back(temp);} 
        temp->setholder(*this);
    }
    virtual void printinfo() {
        if (we[0] != nullptr && we[1] != nullptr)
            cout << "It has a " << we[0]->name << " and a " << we[1]->name << endl;   
    }    
    virtual void hit(soldier * s, weapon * w){
        s->reducehealth(w->damage);
    }
};
class iceman : public soldier{
    public:

    iceman (int sid): soldier(2){
        id = sid;     
    }
    virtual void setweapon(){
        int wid = id % 3;
        weapon * temp;
        if (wid == 0)  {temp = new sword(this); we.push_back(temp);}
        if (wid == 1)  {temp = new bomb(this); we.push_back(temp);}
        if (wid == 2)  {temp = new arrow(this); we.push_back(temp);} 
        temp->setholder(*this);       
    }
    virtual void printinfo(){
        if (we[0] != nullptr) 
            cout << "It has a " << we[0]->name << endl;
    }
    virtual void move();
};
class lion : public soldier{
    private:    int loyalty;
    public:
    lion (int sid, int life): soldier(3){
        id = sid;
        loyalty = life;
    }
    virtual void printinfo(){
        cout << "Its loyalty is " << loyalty << endl;
    }
    virtual void move();
    virtual bool isrunaway(){
        return (loyalty <= 0);
    }
    virtual void setweapon(){
        int wid = id % 3;
        weapon * temp;
        if (wid == 0)  {temp = new sword(this); we.push_back(temp);}
        if (wid == 1)  {temp = new bomb(this); we.push_back(temp);}
        if (wid == 2)  {temp = new arrow(this); we.push_back(temp);} 
        temp->setholder(*this);       
    }
};
vector <weapon*> soldier::canrob(soldier & robber) {
    vector <weapon*> temp;
    sort(we.begin(), we.end(), cmp2);
    
    if (we.empty() || we[0] == nullptr) return temp; 
    
    int idmin = we[0]->wid;
    for (auto e : we) {
        if (e == nullptr) break;

        if (e->wid == idmin && (robber.getWeaponCount() + temp.size() < 10)) {
            temp.push_back(e);
        } else {
            break; 
        }
    }
    return temp;
}
class wolf: public soldier{
    public:
    wolf (int sid) : soldier(4){
        id = sid;
    }
    virtual void robweapon(soldier & s) override{
        if (s.name == "wolf") return;
        vector<weapon*> loot = s.canrob(*this);
        if (loot.empty()) return;
        for (auto e : loot) {
            s.loseWeapon(e);
            e->setholder(*this);
            for (int i = 0; i < 10; i++) {
                if (this->we[i] == nullptr) {
                    this->we[i] = e;
                    break;
                }
            }
            e->setdamage();
        }
        cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " "
            << teamname[team] << " " << name << " " << id <<" took " << loot.size() << " " << loot[0]->name 
            << " from " << teamname[s.team] << " " << s.name << " " << s.id << " in city " << place << endl;
    }
};

void soldier::deleteweapon(weapon * w) {
    for (int i = 0; i < we.size(); i++) {

        if (we[i] == w) {
            delete we[i];
            we[i] = nullptr;
            break;
        }
    }
}
class city{

    public:    
    int cid;
    city () {}
    city (int n): cid(n) {}
    soldier * red{}, * blue{};
    void setsoldier(soldier * s){
        if (s->team == 0) {
            red = s;
        }
        else{
            blue = s;
        }
    }
    void goaway(soldier * s){
        if (s->team == 0) {
            red = nullptr;
        }
        else{
            blue = nullptr;
        }        
    }
    bool isuseup(soldier * s){
        return s->we[0] == nullptr;
    }
    void getweapon() {
        if (red->isalive() and !blue->isalive()){
            sort(blue->we.begin(), blue->we.end(), cmp2);
            int i = 0;
            while (red->getWeaponCount() <= 9 and i < blue->we.size() and blue->we[i] != nullptr){
                for (int k = 0; k < red->we.size(); k++) {
                    if (red->we[k] == nullptr) {
                        red->we[k] = blue->we[i];
                        red->we[k]->setholder(*red);
                        red->we[k]->setdamage();
                        blue->we[i] = nullptr; 
                
                        break;
                    }
                }
                i++;
            }
        }
        if (blue->isalive() and !red->isalive()){
            sort(red->we.begin(), red->we.end(), cmp2);
            int i = 0;
            while (blue->getWeaponCount() <= 9 and i < blue->we.size() and red->we[i] != nullptr){
                for (int k = 0; k < red->we.size(); k++) {
                    if (blue->we[k] == nullptr) {
                        blue->we[k] = red->we[i];
                        blue->we[k]->setholder(*blue);
                        blue->we[k]->setdamage();
                        red->we[i] = nullptr; 
                        break;
                    }
                }
                i++;
            }
        }        
    }
    void fightinfo(){
        if (blue->isalive() and !red->isalive()) {
            sort(blue->we.begin(), blue->we.end(), cmp);
            cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " blue " 
                 << blue->name << " " << blue->id << " killed " << "red " << red->name << " " << red->id << " in city "
                 << cid << " remaining " << blue->gethealth() << " elements" << endl; 

            if (blue->name == "dragon") blue->yell();
        }
        else if (red->isalive() and !blue->isalive()) {
            sort(red->we.begin(), red->we.end(), cmp);
            cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " red " 
                 << red->name << " " << red->id << " killed " << "blue " << blue->name << " " << blue->id << " in city "
                 << cid << " remaining " << red->gethealth() << " elements" << endl; 

            if (red->name == "dragon") red->yell();            
        }
        else if (blue->isalive() and red->isalive()) {
            cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " both red " 
                 << red->name << " " << red->id << " and " << "blue " << blue->name << " " << blue->id << " were alive in city "
                 << cid << endl;
            if (red->name == "dragon") red->yell();
            if (blue->name == "dragon") blue->yell();
        }
        else {
            
            cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " both red " 
                 << red->name << " " << red->id << " and " << "blue " << blue->name << " " << blue->id << " died in city "
                 << cid << endl;       
    
        }
    }
    void fight(){
        if (red == nullptr or blue == nullptr) return;
        sort(red->we.begin(), red->we.end(), cmp);
        sort(blue->we.begin(), blue->we.end(), cmp);
        int redid = 0, blueid = 0;
        int damagered = 0, damageblue = 0;
        bool canfightr = true, canfightb = true;
        if (cid % 2 == 0){
            if (blue->we[blueid] != nullptr){
                blue->hit(red, blue->we[blueid]);                
                damageblue += blue->we[0]->damage;
                blue->we[blueid]->beused();
                blueid++;
            }
        }
        int rround = 0, bround = 0;
        while (red->isalive() and blue->isalive() and (canfightb or canfightr)){  //这里还要考虑都不死的情况（武器用完或攻击力为0）
            sort(blue->we.begin(), blue->we.end(), cmp);
            if (red->we[redid] == nullptr){
                redid = 0;
                if (damagered == 0) {rround++;}
                damagered = 0;
            }
            if(rround >= 5)
                canfightr = false;
            if (!isuseup(red) and redid <= 9 and red->we[redid] != nullptr){
                red->hit(blue, red->we[redid]);
                damagered += red->we[redid]->damage;                
                red->we[redid]->beused();
                redid++;
            }
            sort(red->we.begin(), red->we.end(), cmp);
            if (!blue->isalive()) {
                break;
            }
            if (blue->we[blueid] == nullptr){
                blueid = 0;
                if (damageblue == 0) {bround++;}
                damageblue = 0;
            }
            if(bround >= 5)
                canfightb = false;
            if (!isuseup(blue) and blueid <= 9 and blue->we[blueid] != nullptr){
                blue->hit(red, blue->we[blueid]);
                damageblue += blue->we[blueid]->damage;                            
                blue->we[blueid]->beused();    
                blueid++;
            }     
            if (!red->isalive()) {
                break;
            }
        }
        getweapon();
        fightinfo();
    }


};
city c[25];
soldier::~soldier() {
    for (int i = 0; i <= 9; i++) {
        if (we[i] != nullptr) delete we[i];
    }
}
void initcity(){
    for (int i = 1; i <= citynum; i++){
        c[i].cid = i;
        c[i].red = nullptr;
        c[i].blue = nullptr;
    }
}
void soldier::initplace(){
    if (team == 0) place = 0;
    else place = citynum + 1;
    mycity = &c[place];
}
void soldier::move() {
    if (team == 0) place++;
    else place--;
    mycity->goaway(this);
    mycity = &c[place];
    mycity->setsoldier(this);
}
void iceman::move(){
    int reduce = health / 10;
    reducehealth(reduce);
    soldier::move();
}
void lion::move(){
    loyalty -= k;
    soldier::move();
}
void soldier::printmove(){
    cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " " << teamname[team] << " ";
    cout << name << " " << id << " marched to city " << place << "with " << health << " elements " <<"and "  << "force " << mypower << endl;
}
void soldier::printweapon(){
    int num[3] = {};
    for (auto e: we){
        if (e != nullptr) num[e->wid]++;
    }
    cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " " << teamname[team] << " "
         << name << " " << id << " has " << num[0] << " sword " << num[1] << " bomb " << num[2] << " arrow and "
         << health << " elements" << endl;
}
class base: public city{
    
    int next = 0, mini;
    int eachnum[2] = {0};
    int snum[5] = {0};
    
public:
    bool cancreate = true;
    int total_life;
    string basename;
    vector<soldier*> mysoldiers;
    city * homecity;
    base (string name, int m, city * home): total_life(m), basename(name), homecity(home) {}
    ~base() {
        for (int i = 0; i < mysoldiers.size(); ++i) {
            delete mysoldiers[i];
        }
        mysoldiers.clear(); 
    }
    void Create(){
        int nextone;
        if (basename == "red")  {nextone = listred[next]; sequence = 0;}
        if (basename == "blue")  {nextone = listblue[next]; sequence = 1;}
        if (total_life < healthlist[nextone]) {
            cancreate = false;
			return;
		}
        double morale;
        int loyalty, health = healthlist[nextone];
        string name;
        soldier * temp = nullptr;
        if (nextone == 0) {
            temp = new dragon(++eachnum[sequence], total_life - healthlist[0]);
        }
        else if (nextone == 1) 
        {
            temp = new ninja(++eachnum[sequence]); 
        }
        else if (nextone == 2) 
        {
            temp = new iceman(++eachnum[sequence]); 
        }
        else if (nextone == 3) 
        {
            temp = new lion(++eachnum[sequence], total_life - healthlist[3]); 
        }
        else if (nextone == 4) 
        {
            temp = new wolf(++eachnum[sequence]);
        }
        mysoldiers.push_back(temp);
        temp->team = sequence;
        temp->initplace();
        temp->setweapon();
        snum[nextone]++;
        total_life -= healthlist[nextone];

        cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " " << basename << " ";
        cout << temp->name << " "<< eachnum[sequence] << " born" << endl;
        if (sequence == 0){
            homecity->red = temp;
        }
        else homecity->blue = temp;
        temp->printinfo();
        next = (++next) % 5;
    }
    void runaway(){
        for (int i = 0 ; i <= citynum + 1; i++){

        }
    }
    void afterfight(){
        for (auto it = mysoldiers.begin(); it != mysoldiers.end();) {
            soldier * s = *it;
            if (!s->isalive()){
                delete s;
                mysoldiers.erase(it);
            }
            else{
                it++;
            }
        }
    }
    void deletesoldier(soldier * s){
        auto it = find(mysoldiers.begin(), mysoldiers.end(), s);    
        mysoldiers.erase(it);
        delete s;         
    }
    void printhealth() {
        cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " "
             << total_life << " elements in " << basename << " headquarter" << endl;        
    }
};
int main(){
    int num;
    cin >> num;
    for (int i = 1;i <= num;i++){
        hour = 0, minute = 0;
        int endtime;

        int m;
        cin >> m >> citynum >> k >> endtime;

        cin >> healthlist[0];
        int min = healthlist[0];
        for (int j = 1;j <= 4;j++){
            cin >> healthlist[j];
            if (healthlist[j] < min) min = healthlist[j];
        }
        for (int j = 0; j <= 4; j++){
            cin >> powerlist[j];
        }
        for (int i = 0; i <= citynum + 1; i++) {
            if (c[i].red != nullptr) {
                c[i].red = nullptr;
            }
            if (c[i].blue != nullptr) {
                c[i].blue = nullptr;
            }
        }
        base r("red", m, &c[0]);
        base b("blue", m, &c[citynum + 1]);
        initcity();
        bool is_stop_r = false, is_stop_b = false;
        cout << "Case " << i << ":"  << endl;
        int time = 60 * hour + minute;
        while(time <= endtime){
            // 00分
            if (r.cancreate) {
                r.Create();
            }
            if (b.cancreate) {
                b.Create();
            }            
            // 05分 
            minute = 5; 
            if (time + 5 > endtime) break;             

            for (int i = 0; i <= citynum + 1; i++){
                if (c[i].red != nullptr and c[i].red->isrunaway()) { 
                    cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " red lion " << c[i].red->id << " ran away";
                    cout << endl;
                    soldier* s = c[i].red;
                    c[i].red = nullptr;
                    r.deletesoldier(s);
                }
                if (c[i].blue != nullptr and c[i].blue->isrunaway()) { 
                    cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " blue lion " << c[i].blue->id << " ran away";
                    cout << endl;
                    soldier* s = c[i].blue;
                    c[i].blue = nullptr;
                    b.deletesoldier(s);
                }
            }
            // 10分
            if (time + 10 > endtime) break;
            minute = 10;
            
            for (auto e: r.mysoldiers){
                if (e != nullptr) e->move();
            }
            for (auto e: b.mysoldiers){
                if (e != nullptr) e->move();
            }
            bool stop = false;
            if (r.homecity->blue != nullptr) 
            {
                cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " ";
                cout << "blue " << r.homecity->blue->name << " " << r.homecity->blue->id << " reached red headquarter with " << r.homecity->blue->gethealth() << " elements and force " << r.homecity->blue->getmypower() << endl;
                cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " " ;
                cout << "red headquarter was taken" << endl;
                stop = true;
            }
            for (int i = 1; i <= citynum; i++){
                if (c[i].red != nullptr){                    
                    cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " " ;
                    cout << "red " << c[i].red->name << " " << c[i].red->id << " marched to city " << i <<  " with " << c[i].red->gethealth() << " elements and force " << c[i].red->getmypower() << endl;

                }
                if (c[i].blue != nullptr){
                    cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " ";
                    cout << "blue " << c[i].blue->name << " " << c[i].blue->id << " marched to city " << i <<  " with " << c[i].blue->gethealth() << " elements and force " << c[i].blue->getmypower() << endl;
                
                }
            }
            if (c[citynum + 1].red != nullptr) 
            {
                cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " ";
                cout << "red " << b.homecity->red->name << " " << b.homecity->red->id << " reached blue headquarter with " << b.homecity->red->gethealth() << " elements and force " << b.homecity->red->getmypower() << endl;
                cout << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " " ;
                cout << "blue headquarter was taken" << endl; 
                break;         
            }     
            if (stop) break;
            // 35分
            if (time + 35 > endtime) break;
            
            minute = 35;
            for (int i = 1; i <= citynum; i++){
                
                if (c[i].red != nullptr and c[i].red->name == "wolf" and c[i].blue != nullptr) {
                    c[i].red->robweapon(*c[i].blue);
                }
                if (c[i].blue != nullptr and c[i].blue->name == "wolf" and c[i].red != nullptr) {
                    c[i].blue->robweapon(*c[i].red);
                }
            }

            // 40分
            if (time + 40 > endtime) break;
            minute = 40;
            for (int i = 1; i <= citynum; i++){
                c[i].fight();
            }
            for (int i = 1; i <= citynum; i++) {
                if (c[i].red != nullptr and c[i].red->gethealth() <= 0) {

                    r.deletesoldier(c[i].red); 
                    c[i].red = nullptr;           
                }
                if (c[i].blue != nullptr and c[i].blue->gethealth() <= 0) {
                    b.deletesoldier(c[i].blue);
                    c[i].blue = nullptr; 
                }
            }            
            // 50分
            if (time + 50 > endtime) break;
            minute = 50;
            r.printhealth();
            b.printhealth();
            // 55分
            if (time + 55 > endtime) break;
            minute = 55;

            for (int i = 1; i <= citynum; i++){
                if (c[i].red != nullptr) c[i].red->printweapon();
                if (c[i].blue != nullptr) c[i].blue->printweapon();                
            }

            minute = 0;
            time += 60;
            hour++;
        }
    }
    return 0;
}