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
string namelist[5] = {"dragon", "ninja", "iceman", "lion", "wolf"};
int sequence, citynum;

class weapon;
class city;
class soldier{
protected:    
    int health, id, place, mypower;
public:
    string name;
    int team;
    vector <weapon*> we;

    city * mycity{};
    soldier(int number): name(namelist[number]),health(healthlist[number]), we(10, nullptr){}
    void initplace() ;
    virtual ~soldier(); 
    virtual void setweapon() {};
    void deleteweapon(weapon * w) {
        auto it = std::find(we.begin(), we.end(), w);
        if (it != we.end()) {
            delete *it;
            we.erase(it);
        }
    }
    int gethealth(){
        return health;
    }
    int getmypower(){
        return mypower;
    }
    void hit(soldier * s, weapon * w);
    void reducehealth(int power){
        health -= power;
    }
    bool isalive(){
        if (health <= 0) return false;
        return true;
    }
    void move();
    virtual void printinfo() {}
};

class weapon{
    public:
    int damage, wid;
    string name;
    soldier * holder{};
    weapon (){}
    weapon(soldier *s) {}
    void setholder(soldier & s){
        holder = &s;

    }
    virtual void beused() {}
};
class arrow: public weapon{
    public:
    int use = 0;
    arrow() {name = "arrow";}
    arrow(soldier *s): weapon(s) {name = "arrow"; damage = s->getmypower() * 3 / 10; wid = 2;}
    virtual void beused() {
        use++;
        if (use == 2) holder->deleteweapon(this);
    }
};
class bomb: public weapon{
    public:
    bomb() {name = "bomb";}
    bomb(soldier *s): weapon(s) {name = "bomb"; damage = s->getmypower() * 4 / 10; wid = 1;}
    virtual void beused() {
        holder->deleteweapon(this);
    }
};
class sword: public weapon{

    public:
    sword() {name = "sword";}
    sword(soldier *s): weapon(s) {name = "sword"; damage = s->getmypower() * 2 / 10; wid = 0;}
};
bool cmp(weapon * a, weapon * b) {
    if (!a or !b) return (a != nullptr);
    else {
        return (*a).wid < (*b).wid;
    }
}
void soldier::hit(soldier * s, weapon * w){
    s->reducehealth(w->damage);

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
        if (wid == 0)  {temp = new sword(); we.push_back(new sword());}
        if (wid == 1)  {temp = new bomb(); we.push_back(new bomb());}
        if (wid == 2)  {temp = new arrow(); we.push_back(new arrow());} 
        temp->setholder(*this);       
    }
    virtual void printinfo() {
        if(we[0] != nullptr) {
            cout << "It has a " << we[0]->name << ",and it's morale is " 
                 << fixed << setprecision(2) << morale << endl;
        }
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
        if (wid == 0)  {temp = new sword(); we.push_back(new sword());}
        if (wid == 1)  {temp = new bomb(); we.push_back(new bomb());}
        if (wid == 2)  {temp = new arrow(); we.push_back(new arrow());} 
        temp->setholder(*this);
        if (wid2 == 0)  {temp = new sword(); we.push_back(new sword());}
        if (wid2 == 1)  {temp = new bomb(); we.push_back(new bomb());}
        if (wid2 == 2)  {temp = new arrow(); we.push_back(new arrow());} 
        temp->setholder(*this);
    }
    virtual void printinfo() {
        if (we[0] != nullptr && we[1] != nullptr)
            cout << "It has a " << we[0]->name << " and a " << we[1]->name << endl;   
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
        if (wid == 0)  {temp = new sword(); we.push_back(new sword());}
        if (wid == 1)  {temp = new bomb(); we.push_back(new bomb());}
        if (wid == 2)  {temp = new arrow(); we.push_back(new arrow());} 
        temp->setholder(*this);       
    }
    virtual void printinfo(){
        if (we[0] != nullptr) 
            cout << "It has a " << we[0]->name << endl;
    }
};
class lion : public soldier{
    private:    int loyalty;
    public:
    lion (int sid, int life): soldier(3){
        id = sid;
        loyalty = life;
    }
    virtual void printinfo(){
        cout << "It's loyalty is " << loyalty << endl;
    }

};
class wolf: public soldier{
    public:
    wolf (int sid) : soldier(4){
        id = sid;
    }
};
soldier::~soldier() {
    for (int i = 0; i <= 9; i++) {
        if (we[i] != nullptr) delete we[i];
    }
    if (mycity != nullptr) delete mycity;
}


class city{

    public:    
    int cid;
    city () {}
    city (int n): cid(n) {}
    soldier * red{}, * blue{};
    void setsoldier(soldier s){
        if (s.team == 0) {
            red = &s;
        }
        else{
            blue = &s;
        }
    }
    bool isuseup(soldier * s){
        return s->we[0] == nullptr;
    }
    void fight(){
        if (red == nullptr or blue == nullptr) return;
        sort(red->we.begin(), red->we.end(), cmp); //这里arrow类还没排好序
        int redid = 0, blueid = 0;
        int damagered = 0, damageblue = 0;
        bool canfightr = true, canfightb = true;
        if (cid % 2 == 0){
            if (blue->we[blueid] != nullptr){
                blue->hit(red, blue->we[blueid]);
                blueid++;
                damageblue += blue->we[0]->damage;
            }
        }
        while (red->isalive() and blue->isalive() and (canfightb or canfightr)){  //这里还要考虑都不死的情况（武器用完或攻击力为0）

            if (red->we[redid] == nullptr){
                redid = 0;
                if (damagered == 0) canfightr = false;
                damagered = 0;
            }
            if (!isuseup(red) and redid <= 9 and red->we[redid] != nullptr){
                red->hit(blue, red->we[redid]);
                damagered += red->we[redid]->damage;
                redid++;
            }
            if (!blue->isalive()) break;
            if (blue->we[blueid] == nullptr){
                blueid = 0;
                if (damageblue == 0) canfightb = false;
                damageblue = 0;
            }
            if (!isuseup(blue) and blueid <= 9 and blue->we[blueid] != nullptr){
                blue->hit(red, blue->we[blueid]);
                damageblue += blue->we[blueid]->damage;                
                blueid++;
            }     
        }
    }
    void fightinfo(){
        if (red == nullptr and blue != nullptr) {}
        else if (blue == nullptr and red != nullptr) {}
        else if (red == nullptr and blue == nullptr) {}
        else {}
    }
};
void soldier::initplace(){
    if (team == 0) place = 0;
    else place = citynum + 1;
    mycity = &city(place);
}
void soldier::move() {
    place++;
    mycity = &city(place);
    mycity->setsoldier(*this);
}
class base{
    
    int next = 0, mini;
    int eachnum[2] = {0};
    int snum[5] = {0};
public:
    int total_life;
    string basename;
    vector<soldier*> mysoldiers;
    base (string name, int m, int min): total_life(m), basename(name),  mini(min){}
    ~base() {
        for (int i = 0; i < mysoldiers.size(); ++i) {
            delete mysoldiers[i];
        }
        mysoldiers.clear(); 
    }
    void Create(int t){
        int nextone;
        if (basename == "red")  {nextone = listred[next]; sequence = 0;}
        if (basename == "blue")  {nextone = listblue[next]; sequence = 1;}
        if (total_life < healthlist[nextone]) {
			next = (++next) % 5;
			Create(t);
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

        cout << setw(3) << setfill('0') << t << " " << basename << " " ;
        cout << temp->name << " "<< eachnum[sequence] << " ";
        cout << "born with strength " << temp->gethealth() << ',' << snum[nextone] << " " << temp->name << " ";
        cout << "in " << basename << " headquarter" << endl;
        temp->printinfo();
        next = (++next) % 5;
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
};
int main(){
    int num;
    city c[22];
    cin >> num;
    for (int i = 1;i <= num;i++){
        int time = 0;

        int m;
        cin >> m >> citynum;
        for (int j = 1; j <= citynum; j++) {
            c[i].cid = j;
        }
        cin >> healthlist[0];
        int min = healthlist[0];
        for (int j = 1;j <= 4;j++){
            cin >> healthlist[j];
            if (healthlist[j] < min) min = healthlist[j];
        }
        base r("red", m, min);
        base b("blue", m, min);
        bool is_stop_r = false, is_stop_b = false;
        cout << "Case:" << i << endl;
        while (!(is_stop_b and is_stop_r)){
            if (r.total_life >= min){
                r.Create(time);
			}
            else if(is_stop_r == false){
                cout << setw(3) << setfill('0') << time << " red headquarter stops making warriors" << endl;
                is_stop_r = true;
			}
            if (b.total_life >= min)
                b.Create(time);
            else if(is_stop_b == false){
                cout << setw(3) << setfill('0') << time << " blue headquarter stops making warriors" << endl;
                is_stop_b = true;    
			}            
            time++;
        }
    }
    return 0;
}