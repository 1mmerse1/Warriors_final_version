

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>
#include <algorithm>
using namespace std;

// Logger
struct Logger {
    static ostream& log(int hour, int minute) {
        return cout << setfill('0') << setw(3) << hour
                    << ":" << setw(2) << minute << " ";
    }
};

// 静态全局配置（每个 case 重新赋值）
static int healthlist[5] = {};
static int powerlist[5]  = {};
static const string namelist[5] = {"dragon","ninja","iceman","lion","wolf"};
static const string teamname[2] = {"red","blue"};
static int listred[5]  = {2,3,4,1,0};
static int listblue[5] = {3,0,1,2,4};
static int r = 0;   // arrow 固定伤害
static int k = 0;   // lion loyalty 每次战斗不胜降低量

// 前向声明
class Weapon; class Arrow; class Bomb; class Sword;
class City; class Soldier; class Base;
class Dragon; class Ninja; class Iceman; class Lion; class Wolf; 
// Weapon 基类

class Weapon {
public:
    int damage{}, wid{}, use{};
    string name;
    Soldier* holder{};

    Weapon() = default;
    explicit Weapon(Soldier* s) : holder(s) {}
    virtual ~Weapon() = default;

    void setholder(Soldier& s) { holder = &s; }
    virtual bool beused() { return false; }
};


// Soldier 基类
class Soldier {
protected:
    int health{}, mypower{};
public:
    string name;
    int team{}, place{}, id{};
    double morale{};
    bool arrived{};  // 已到达敌方司令部，不再行动

    unique_ptr<Sword>  mySword;
    unique_ptr<Bomb>   myBomb;
    unique_ptr<Arrow>  myArrow;
    City* mycity{};

    explicit Soldier(int number)
        : name(namelist[number]),
          health(healthlist[number]),
          mypower(powerlist[number]) {}

    virtual ~Soldier() = default;

    void initplace(City* home);
    virtual void setweapon() {}

    int gethealth()  const { return health; }
    int getmypower() const { return mypower; }
    int getplace()   const { return place; }

    virtual double getmorale()  const { return 0.0; }
    virtual int    getloyalty() const { return 0; }

    // 主动攻击：dmg = power + sword
    // 反击：dmg = floor(power/2) + sword
    void hitby(Soldier* attacker, bool isCounterAttack);
    void hitwitharrow(Soldier* s);

    void reducehealth(int v) {
        health -= v;
        if (health < 0) health = 0;
    }
    void raisehealth(int v) { health += v; }

    bool isalive()           const { return health > 0; }
    virtual bool isrunaway() const { return false; }
    virtual void mymove(int citynum);
    virtual void addmorale(double d) {}
    virtual void loyaltydown(int d) {}
    virtual void printinfo() {}
    void printweapon(int hour, int minute) const;
};

// Weapon 子类
class Arrow : public Weapon {
public:
    Arrow() { name="arrow"; wid=2; }
    explicit Arrow(Soldier* s) : Weapon(s) { name="arrow"; wid=2; damage=r; }
    bool beused() override { return ++use >= 3; }
};

class Bomb : public Weapon {
public:
    Bomb() { name="bomb"; wid=1; }
    explicit Bomb(Soldier* s) : Weapon(s) {
        name="bomb"; wid=1;
        damage = s->getmypower() * 4 / 10;
    }
    bool beused() override { return true; }
};

class Sword : public Weapon {
public:
    Sword() { name="sword"; wid=0; }
    explicit Sword(Soldier* s) : Weapon(s) {
        name="sword"; wid=0;
        damage = s->getmypower() * 2 / 10;
    }
    bool beused() override {
        damage = damage * 8 / 10;
        return (damage == 0);
    }
};

// 武器工厂（sword 初始攻击力为 0 则不创建）
void makeWeapon(int wid, Soldier* owner) {
    if (wid == 0) {
        int dmg = owner->getmypower() * 2 / 10;
        if (dmg > 0) {
            owner->mySword = make_unique<Sword>(owner);
            owner->mySword->setholder(*owner);
        }
    } else if (wid == 1) {
        owner->myBomb = make_unique<Bomb>(owner);
        owner->myBomb->setholder(*owner);
    } else {
        owner->myArrow = make_unique<Arrow>(owner);
        owner->myArrow->setholder(*owner);
    }
}

// 被 attacker 攻击（isCounterAttack=true 时是反击）
void Soldier::hitby(Soldier* atk, bool isCounterAttack) {
    int dmg = isCounterAttack ? (atk->getmypower() / 2) : atk->getmypower();
    if (atk->mySword) dmg += atk->mySword->damage;
    reducehealth(dmg);
    // atk 的剑衰减
    if (atk->mySword && atk->mySword->beused()) atk->mySword.reset();
}

void Soldier::hitwitharrow(Soldier* s) {
    if (!myArrow || !s) return;
    s->reducehealth(r);
    if (myArrow->beused()) myArrow.reset();
}

void Soldier::mymove(int citynum) {
    // 已到达敌方司令部则不再移动
    if (team == 0 && place >= citynum + 1) { arrived = true; return; }
    if (team == 1 && place <= 0)           { arrived = true; return; }
    place += (team == 0) ? 1 : -1;
    // 检查是否刚到达敌方司令部
    if (team == 0 && place == citynum + 1) arrived = true;
    if (team == 1 && place == 0)           arrived = true;
}

// 武器报告：arrow(N),bomb,sword(N) 或 has no weapon
void Soldier::printweapon(int hour, int minute) const {
    Logger::log(hour, minute)
        << teamname[team] << " " << name << " " << id << " has ";

    string parts[3];
    int cnt = 0;
    if (myArrow) {
        ostringstream oss;
        oss << "arrow(" << (3 - myArrow->use) << ")";
        parts[cnt++] = oss.str();
    }
    if (myBomb)  parts[cnt++] = "bomb";
    if (mySword) {
        ostringstream oss;
        oss << "sword(" << mySword->damage << ")";
        parts[cnt++] = oss.str();
    }

    if (cnt == 0) {
        cout << "no weapon\n";
    } else {
        for (int i = 0; i < cnt; i++) {
            if (i) cout << ",";
            cout << parts[i];
        }
        cout << "\n";
    }
}

// City 类
class City {
public:
    int cid{};
    Soldier* red{};
    Soldier* blue{};
    int mylife   = 0;
    int lastwinner = -1;   // -1=无, 0=红, 1=蓝
    int flag   = -1;       // -1=无旗, 0=红旗, 1=蓝旗
    int redwin = 0, bluewin = 0;
    bool bomb_killed = false;  // 本轮炸弹击杀，:40 跳过

    City() = default;
    explicit City(int n) : cid(n) {}

    void setsoldier(Soldier* s) {
        if (!s) return;
        if (s->team == 0) red  = s;
        else              blue = s;
    }

    // 单人取走城市生命元（:30）
    int earnAlone(Base* rBase, Base* bBase);

    // 主战斗（:40）
    void fight(int hour, int minute, Base* rBase, Base* bBase);
};
void Soldier::initplace(City* home) {
    mycity = home;
    place  = home->cid;
}


// Soldier 子类
class Dragon : public Soldier {
public:
    Dragon(int sid, int life) : Soldier(0) {
        morale = (double)life / (double)healthlist[0];
        id = sid;
    }
    void setweapon() override { makeWeapon(id % 3, this); }
    void printinfo() override {
        cout << "Its morale is "
             << fixed << setprecision(2) << morale << "\n";
    }
    double getmorale() const override { return morale; }
    void   addmorale(double d) override { morale += d; }
    void   yell(int hour, int minute) const {
        Logger::log(hour, minute)
            << teamname[team] << " dragon " << id
            << " yelled in city " << place << "\n";
    }
};

class Ninja : public Soldier {
public:
    explicit Ninja(int sid) : Soldier(1) { id = sid; }
    void setweapon() override {
        makeWeapon( id      % 3, this);
        makeWeapon((id + 1) % 3, this);
    }

};

class Iceman : public Soldier {
    int movecount = 0;
public:
    explicit Iceman(int sid) : Soldier(2) { id = sid; }
    void setweapon() override { makeWeapon(id % 3, this); }

    void mymove(int citynum) override {
        if (++movecount == 2) {
            movecount = 0;
            if (health > 9) health -= 9; else health = 1;
            mypower += 20;
        }
        Soldier::mymove(citynum);
    }
};

class Lion : public Soldier {
    int loyalty{};
public:
    Lion(int sid, int life) : Soldier(3), loyalty(life) { id = sid; }
    void printinfo() override { cout << "Its loyalty is " << loyalty << "\n"; }
    int  getloyalty() const override { return loyalty; }
    void loyaltydown(int d) override { loyalty -= d; }
    void mymove(int citynum) override { Soldier::mymove(citynum); }
    bool isrunaway() const override { return loyalty <= 0; }
};

class Wolf : public Soldier {
public:
    explicit Wolf(int sid) : Soldier(4) { id = sid; }
    // Wolf 出生时无武器，无 printinfo 输出
};

// Base 类
class Base {
    int next    = 0;
    int eachnum = 0;
    int snum[5] = {};
    const int* spawnList;
    int teamSeq;
    int m_k;

public:
    bool cancreate = true;
    int  total_life;
    string basename;
    vector<unique_ptr<Soldier>> mysoldiers;
    City* homecity{};

    Base(string name, int m, City* home, const int* list, int seq, int k_val)
        : spawnList(list), teamSeq(seq), m_k(k_val),
          total_life(m), basename(move(name)), homecity(home) {}

    ~Base() = default;

    void Create(int hour, int minute) {
        int nextone = spawnList[next];
        if (total_life < healthlist[nextone]) return;

        int sid = ++eachnum;
        unique_ptr<Soldier> temp;
        if      (nextone == 0) temp = make_unique<Dragon>(sid, total_life - healthlist[0]);
        else if (nextone == 1) temp = make_unique<Ninja>(sid);
        else if (nextone == 2) temp = make_unique<Iceman>(sid);
        else if (nextone == 3) temp = make_unique<Lion>(sid, total_life - healthlist[3]);
        else                   temp = make_unique<Wolf>(sid);

        temp->team = teamSeq;
        temp->initplace(homecity);
        temp->setweapon();
        snum[nextone]++;
        total_life -= healthlist[nextone];

        Logger::log(hour, minute)
            << basename << " " << temp->name << " " << sid << " born\n";

        Soldier* raw = temp.get();
        mysoldiers.push_back(move(temp));

        if (teamSeq == 0) homecity->red  = raw;
        else              homecity->blue = raw;

        raw->printinfo(); // Dragon 打印morale，Lion 打印 loyalty，等
        next = (next + 1) % 5;
    }

    void deletesoldier(Soldier* s) {
        mysoldiers.erase(
            remove_if(mysoldiers.begin(), mysoldiers.end(),
                [s](const unique_ptr<Soldier>& p){ return p.get() == s; }),
            mysoldiers.end());
    }

    void printhealth(int hour, int minute) const {
        Logger::log(hour, minute)
            << total_life << " elements in " << basename << " headquarter\n";
    }

    // 奖励胜利武士（距离敌方司令部近的优先）
    void giveReward(Soldier* s) {
        if (s && total_life >= 8) {
            s->raisehealth(8);
            total_life -= 8;
        }
    }

    vector<Soldier*> getSoldiers() const {
        vector<Soldier*> v;
        v.reserve(mysoldiers.size());
        for (const auto& sp : mysoldiers) v.push_back(sp.get());
        return v;
    }

};
// City::earnAlone（:30 单人取生命元）
int City::earnAlone(Base* rBase, Base* bBase) {
    // 必须恰好只有一方武士
    bool hasRed  = (red  != nullptr);
    bool hasBlue = (blue != nullptr);
    if (hasRed == hasBlue) return 0;

    Soldier* s   = hasRed ? red : blue;
    Base*    hq  = hasRed ? rBase : bBase;
    int      earned = mylife;
    if (earned <= 0) return 0;

    hq->total_life += earned;
    mylife = 0;
    return earned;  // 调用方输出
}

// City::fight（:40 主战斗）

void City::fight(int hour, int minute, Base* rBase, Base* bBase) {
    Soldier* rs = red;
    Soldier* bs = blue;

    // 炸弹击杀：不算战斗
    if (bomb_killed) {
        bomb_killed = false;
        lastwinner = -1;
        return;
    }

    if (!rs || !bs) return;

    bool r_alive = rs->isalive();
    bool b_alive = bs->isalive();

    if (!r_alive && !b_alive) { lastwinner = -1; return; }

    bool redFirst = (flag == 0) || (flag == -1 && cid % 2 == 1);
    Soldier* atk = redFirst ? rs : bs;
    Soldier* def = redFirst ? bs : rs;

    bool melee = r_alive && b_alive;

    // Lion 战斗前生命值
    int atk_lion_hp = (atk->name == "lion") ? atk->gethealth() : 0;
    int def_lion_hp = (def->name == "lion") ? def->gethealth() : 0;

    // 先手攻击
    if (melee) {
        Logger::log(hour, minute)
            << teamname[atk->team] << " " << atk->name << " " << atk->id
            << " attacked " << teamname[def->team] << " " << def->name << " " << def->id
            << " in city " << cid
            << " with " << atk->gethealth() << " elements and force " << atk->getmypower() << "\n";

        def->hitby(atk, false);

        // 反击
        if (def->isalive() && def->name != "ninja") {
            Logger::log(hour, minute)
                << teamname[def->team] << " " << def->name << " " << def->id
                << " fought back against "
                << teamname[atk->team] << " " << atk->name << " " << atk->id
                << " in city " << cid << "\n";
            atk->hitby(def, true);   // 反击
        }

        // 被杀死的善后
        if (!def->isalive()) {
            Logger::log(hour, minute)
                << teamname[def->team] << " " << def->name << " " << def->id
                << " was killed in city " << cid << "\n";
            // Lion 生命值转移给活着的对手
            if (def->name == "lion" && atk->isalive())
                atk->raisehealth(def_lion_hp);
        }
        if (!atk->isalive()) {
            Logger::log(hour, minute)
                << teamname[atk->team] << " " << atk->name << " " << atk->id
                << " was killed in city " << cid << "\n";
            if (atk->name == "lion" && def->isalive())
                def->raisehealth(atk_lion_hp);
        }
    }

    // 确定胜负
    Soldier* winner = nullptr;
    Soldier* loser  = nullptr;
    if (!melee) {
        // 一方已被箭射死
        winner = r_alive ? rs : bs;
        loser  = r_alive ? bs : rs;
    } else {
        bool atk_ok = atk->isalive();
        bool def_ok = def->isalive();
        if (atk_ok && !def_ok) { winner = atk; loser = def; }
        else if (!atk_ok && def_ok) { winner = def; loser = atk; }
    }

    // Dragon 士气更新（欢呼前）
    if (atk->name == "dragon") {
        if (winner == atk) atk->addmorale(0.2);
        else               atk->addmorale(-0.2);
    }
    if (def->name == "dragon" && def->isalive()) {
        if (winner == def) def->addmorale(0.2);
        else               def->addmorale(-0.2);
    }

    // Dragon 欢呼
    if (atk->name == "dragon" && atk->isalive() && atk->getmorale() > 0.8) {
        static_cast<Dragon*>(atk)->yell(hour, minute);
    }

    // Wolf 缴获武器
    if (winner && winner->name == "wolf" && loser) {
        if (!winner->mySword  && loser->mySword)  winner->mySword  = move(loser->mySword);
        if (!winner->myBomb   && loser->myBomb)   winner->myBomb   = move(loser->myBomb);
        if (!winner->myArrow  && loser->myArrow)  winner->myArrow  = move(loser->myArrow);
    }

    // 胜者取走城市生命元
    if (winner && mylife > 0) {
        int earned = mylife;
        mylife = 0;
        if (winner->team == 0) rBase->total_life += earned;
        else                   bBase->total_life += earned;
        Logger::log(hour, minute)
            << teamname[winner->team] << " " << winner->name << " " << winner->id
            << " earned " << earned << " elements for his headquarter\n";
    }

    // 更新连胜计数
    if (winner == rs) {
        redwin++; bluewin = 0; lastwinner = 0;
    } else if (winner == bs) {
        bluewin++; redwin = 0; lastwinner = 1;
    } else {
        redwin = 0; bluewin = 0; lastwinner = -1;
    }

    // 换旗子
    if (redwin >= 2 && flag != 0) {
        flag = 0;
        Logger::log(hour, minute) << "red flag raised in city " << cid << "\n";
    } else if (bluewin >= 2 && flag != 1) {
        flag = 1;
        Logger::log(hour, minute) << "blue flag raised in city " << cid << "\n";
    }

    // Lion 忠诚度：只在近战中未胜的一方降低
    if (melee) {
        if (rs->name == "lion" && rs->isalive() && winner != rs) rs->loyaltydown(k);
        if (bs->name == "lion" && bs->isalive() && winner != bs) bs->loyaltydown(k);
    }
}

// World 类
class World {
private:
    int m_cityNum;
    int m_k;
    int m_limit;
    int m_hour;

    vector<City>     m_cities;
    unique_ptr<Base> m_red;
    unique_ptr<Base> m_blue;

    // 到达敌方司令部的敌人计数
    int m_blues_in_red_hq = 0;
    int m_reds_in_blue_hq = 0;

    bool checkTime(int minute) const {
        return (m_hour * 60 + minute) <= m_limit;
    }
    ostream& log(int minute) const { return Logger::log(m_hour, minute); }

    // 清除死亡士兵（:40 后统一调用）
    void cleanupDead() {
        for (int i = 0; i <= m_cityNum + 1; i++) {
            if (m_cities[i].red && !m_cities[i].red->isalive()) {
                m_red->deletesoldier(m_cities[i].red);
                m_cities[i].red = nullptr;
            }
            if (m_cities[i].blue && !m_cities[i].blue->isalive()) {
                m_blue->deletesoldier(m_cities[i].blue);
                m_cities[i].blue = nullptr;
            }
        }
    }

    // 纯数值预测战斗是否能活（供炸弹判断）
    bool wouldSurvive(int cityid, Soldier* me, Soldier* enemy) {
        bool redFirst = (m_cities[cityid].flag == 0) ||
                        (m_cities[cityid].flag == -1 && cityid % 2 == 1);
        bool iAmFirst = (me->team == 0) ? redFirst : !redFirst;

        int myAtk  = me->getmypower()    + (me->mySword    ? me->mySword->damage    : 0);
        int enAtk  = enemy->getmypower() + (enemy->mySword ? enemy->mySword->damage : 0);
        int enBack = enemy->getmypower()/2 + (enemy->mySword ? enemy->mySword->damage : 0);

        if (iAmFirst) {
            if (myAtk >= enemy->gethealth()) return true;
            if (enemy->name == "ninja")      return true;  // ninja 不反击
            return me->gethealth() > enBack;
        } else {
            return me->gethealth() > enAtk;
        }
    }

    // 发奖励
    void rewardWinners() {
        // 红方：从城市 N 到 1
        for (int i = m_cityNum; i >= 1; i--) {
            if (m_cities[i].lastwinner == 0 && m_cities[i].red) {
                m_red->giveReward(m_cities[i].red);
                if (m_red->total_life < 8) break;
            }
        }
        // 蓝方：从城市 1 到 N
        for (int i = 1; i <= m_cityNum; i++) {
            if (m_cities[i].lastwinner == 1 && m_cities[i].blue) {
                m_blue->giveReward(m_cities[i].blue);
                if (m_blue->total_life < 8) break;
            }
        }
    }

    void event_00_spawn();
    void event_05_lionRun();
    bool event_10_march();   // 返回 true 表示游戏结束
    void event_20_harvest();
    void event_30_earn();
    void event_35_arrowattack();
    void event_38_bombjudge();
    void event_40_battle();
    void event_50_reportBase();
    void event_55_reportSoldier();

public:
    World(int m, int cityNum, int k_val, int limit)
        : m_cityNum(cityNum), m_k(k_val), m_limit(limit), m_hour(0),
          m_cities(cityNum + 2)
    {
        for (int i = 0; i <= cityNum + 1; i++) m_cities[i].cid = i;
        m_red  = make_unique<Base>("red",  m, &m_cities[0], listred,  0, k_val);
        m_blue = make_unique<Base>("blue", m, &m_cities[cityNum + 1], listblue, 1, k_val);
    }

    void run() {
        while (true) {
            if (!checkTime(0))  break; 
            event_00_spawn();
            if (!checkTime(5))  break; 
            event_05_lionRun();
            if (!checkTime(10)) break; 
            if (event_10_march()) break;
            if (!checkTime(20)) break; 
            event_20_harvest();
            if (!checkTime(30)) break; 
            event_30_earn();
            if (!checkTime(35)) break; 
            event_35_arrowattack();
            if (!checkTime(38)) break; 
            event_38_bombjudge();
            if (!checkTime(40)) break; 
            event_40_battle();
            if (!checkTime(50)) break; 
            event_50_reportBase();
            if (!checkTime(55)) break; 
            event_55_reportSoldier();
            m_hour++;
        }
    }
};

// 事件实现

void World::event_00_spawn() {
    Soldier* prev_r = m_cities[0].red;
    Soldier* prev_b = m_cities[m_cityNum+1].blue;

    m_red->Create(m_hour, 0);
    m_blue->Create(m_hour, 0);

    // 将新生武士放置到各自的出发城市
    auto setHome = [](Base& base, City& home, bool isRed) {
        if (!base.mysoldiers.empty()) {
            Soldier* s = base.mysoldiers.back().get();
            if (!s->mycity) {   // 未初始化说明刚出生
                s->initplace(&home);
                s->mycity = &home;
                if (isRed) home.red  = s;
                else       home.blue = s;
            }
        }
    };

    for (auto& sp : m_red->mysoldiers) {
        if (sp->place == 0 && !sp->arrived && m_cities[0].red != sp.get()) {
            sp->mycity = &m_cities[0];
            m_cities[0].red = sp.get();
        }
    }
    for (auto& sp : m_blue->mysoldiers) {
        if (sp->place == 0 && !sp->arrived) {
            sp->place = m_cityNum + 1;
            sp->mycity = &m_cities[m_cityNum + 1];
            m_cities[m_cityNum + 1].blue = sp.get();
        }
    }
}

void World::event_05_lionRun() {
    for (int i = 0; i <= m_cityNum + 1; i++) {
        // 已到达敌方司令部的 lion 不逃跑
        if (m_cities[i].red && m_cities[i].red->name == "lion" &&
            m_cities[i].red->isrunaway() && i != m_cityNum + 1)
        {
            log(5) << "red lion " << m_cities[i].red->id << " ran away\n";
            Soldier* s = m_cities[i].red;
            m_cities[i].red = nullptr;
            m_red->deletesoldier(s);
        }
    }
    for (int i = 0; i <= m_cityNum + 1; i++) {
        if (m_cities[i].blue && m_cities[i].blue->name == "lion" &&
            m_cities[i].blue->isrunaway() && i != 0)
        {
            log(5) << "blue lion " << m_cities[i].blue->id << " ran away\n";
            Soldier* s = m_cities[i].blue;
            m_cities[i].blue = nullptr;
            m_blue->deletesoldier(s);
        }
    }
}

bool World::event_10_march() {
    for (int i = 0; i <= m_cityNum + 1; i++) {
        m_cities[i].red  = nullptr;
        m_cities[i].blue = nullptr;
    }

    Soldier* newBlueInRedHQ = nullptr;
    Soldier* newRedInBlueHQ = nullptr;

    // 并捕捉刚到站的武士
    for (Soldier* s : m_red->getSoldiers()) {
        int oldPlace = s->place;
        s->mymove(m_cityNum);
        if (oldPlace == m_cityNum && s->place == m_cityNum + 1) {
            newRedInBlueHQ = s;
        }
    }
    for (Soldier* s : m_blue->getSoldiers()) {
        int oldPlace = s->place;
        s->mymove(m_cityNum);
        if (oldPlace == 1 && s->place == 0) {
            newBlueInRedHQ = s;
        }
    }

    for (Soldier* s : m_red->getSoldiers()) {
        int p = s->place;
        if (p >= 0 && p <= m_cityNum + 1) {
            m_cities[p].setsoldier(s);
            s->mycity = &m_cities[p];
        }
    }
    for (Soldier* s : m_blue->getSoldiers()) {
        int p = s->place;
        if (p >= 0 && p <= m_cityNum + 1) {
            m_cities[p].setsoldier(s);
            s->mycity = &m_cities[p];
        }
    }

    // 检查是否有蓝武士到达红司令部
    bool gameover = false;

    if (newBlueInRedHQ) {
        Soldier* inv = newBlueInRedHQ;
        log(10) << "blue " << inv->name << " " << inv->id
                << " reached red headquarter with "
                << inv->gethealth() << " elements and force "
                << inv->getmypower() << "\n";
        m_blues_in_red_hq++;
        if (m_blues_in_red_hq >= 2) {
            log(10) << "red headquarter was taken\n";
            // 输出剩余城市的行军

            gameover = true;
        }
    }

    for (int i = 1; i <= m_cityNum; i++) {
        if (m_cities[i].red)
            log(10) << "red " << m_cities[i].red->name << " " << m_cities[i].red->id
                    << " marched to city " << i << " with "
                    << m_cities[i].red->gethealth() << " elements and force "
                    << m_cities[i].red->getmypower() << "\n";
        if (m_cities[i].blue)
            log(10) << "blue " << m_cities[i].blue->name << " " << m_cities[i].blue->id
                    << " marched to city " << i << " with "
                    << m_cities[i].blue->gethealth() << " elements and force "
                    << m_cities[i].blue->getmypower() << "\n";
    }    



    // 红方到达蓝方司令部
    if (newRedInBlueHQ) {
        Soldier* inv = newRedInBlueHQ;
        log(10) << "red " << inv->name << " " << inv->id
                << " reached blue headquarter with "
                << inv->gethealth() << " elements and force "
                << inv->getmypower() << "\n";
        m_reds_in_blue_hq++;
        if (m_reds_in_blue_hq >= 2) {
            log(10) << "blue headquarter was taken\n";
            gameover = true;
        }
    }

    return gameover;
}

void World::event_20_harvest() {
    for (int i = 1; i <= m_cityNum; i++)
        m_cities[i].mylife += 10;
}

void World::event_30_earn() {
    for (int i = 1; i <= m_cityNum; i++) {
        bool hasRed  = (m_cities[i].red  != nullptr);
        bool hasBlue = (m_cities[i].blue != nullptr);
        if (hasRed == hasBlue) continue;

        Soldier* s  = hasRed ? m_cities[i].red : m_cities[i].blue;
        Base*    hq = hasRed ? m_red.get()      : m_blue.get();
        int      earned = m_cities[i].mylife;
        if (earned <= 0) continue;

        hq->total_life += earned;
        m_cities[i].mylife = 0;
        log(30) << teamname[s->team] << " " << s->name << " " << s->id
                << " earned " << earned << " elements for his headquarter\n";
    }
}

void World::event_35_arrowattack() {
    // 事件发生在射箭城市
    for (int i = 1; i <= m_cityNum; i++) {
        // 红武士在 city i 向东射
        if (m_cities[i].red && m_cities[i].red->myArrow) {
            Soldier* shooter = m_cities[i].red;
            Soldier* target  = (i + 1 <= m_cityNum) ? m_cities[i+1].blue : nullptr;
            if (target) {
                bool killed = (target->gethealth() <= r);
                shooter->hitwitharrow(target);
                if (killed) {
                    log(35) << "red " << shooter->name << " " << shooter->id
                            << " shot and killed blue " << target->name << " " << target->id << "\n";
                } else {
                    log(35) << "red " << shooter->name << " " << shooter->id << " shot\n";
                }
            }
        }

        // 蓝武士在 city i 向西射
        if (m_cities[i].blue && m_cities[i].blue->myArrow) {
            Soldier* shooter = m_cities[i].blue;
            Soldier* target  = (i - 1 >= 1) ? m_cities[i-1].red : nullptr;
            if (target) {
                bool killed = (target->gethealth() <= r);
                shooter->hitwitharrow(target);
                if (killed) {
                    log(35) << "blue " << shooter->name << " " << shooter->id
                            << " shot and killed red " << target->name << " " << target->id << "\n";
                } else {
                    log(35) << "blue " << shooter->name << " " << shooter->id << " shot\n";
                }
            }
        }
    }
}

void World::event_38_bombjudge() {
    for (int i = 1; i <= m_cityNum; i++) {
        Soldier* rs = m_cities[i].red;
        Soldier* bs = m_cities[i].blue;
        if (!rs || !bs) continue;
        if (!rs->isalive() || !bs->isalive()) continue;

        bool redFirst = (m_cities[i].flag == 0) ||
                        (m_cities[i].flag == -1 && i % 2 == 1);
        Soldier* first  = redFirst ? rs : bs;
        Soldier* second = redFirst ? bs : rs;

        // 先手方先判断
        auto tryBomb = [&](Soldier* me, Soldier* enemy) -> bool {
            if (!me->myBomb) return false;
            if (wouldSurvive(i, me, enemy)) return false;
            // 使用炸弹
            log(38) << teamname[me->team] << " " << me->name << " " << me->id
                    << " used a bomb and killed "
                    << teamname[enemy->team] << " " << enemy->name << " " << enemy->id << "\n";
            me->reducehealth(me->gethealth() + 1);    // 强制死亡
            enemy->reducehealth(enemy->gethealth() + 1);
            me->myBomb.reset();
            m_cities[i].bomb_killed = true;
            return true;
        };

        if (tryBomb(first, second)) continue;
        tryBomb(second, first);
    }
    // 死亡士兵不在此处清理
}

void World::event_40_battle() {
    // 重置 lastwinner
    for (int i = 1; i <= m_cityNum; i++) m_cities[i].lastwinner = -1;

    for (int i = 1; i <= m_cityNum; i++)
        m_cities[i].fight(m_hour, 40, m_red.get(), m_blue.get());

    // 发奖励
    rewardWinners();

    // 清除所有死亡士兵
    cleanupDead();
}

void World::event_50_reportBase() {
    m_red->printhealth(m_hour, 50);
    m_blue->printhealth(m_hour, 50);
}

void World::event_55_reportSoldier() {
    for (int i = 1; i <= m_cityNum + 1; i++)
        if (m_cities[i].red)  m_cities[i].red->printweapon(m_hour, 55);
    for (int i = 0; i <= m_cityNum; i++)
        if (m_cities[i].blue) m_cities[i].blue->printweapon(m_hour, 55);
}

int main() {
    freopen("Warcraft.in", "r", stdin);
    freopen("my_output.txt", "w", stdout);

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int num;
    cin >> num;

    for (int cas = 1; cas <= num; cas++) {
        int m, cityNum, endtime;
        cin >> m >> cityNum >> r >> k >> endtime;

        for (int j = 0; j < 5; j++) cin >> healthlist[j];
        for (int j = 0; j < 5; j++) cin >> powerlist[j];

        cout << "Case " << cas << ":\n";

        World world(m, cityNum, k, endtime);
        world.run();
    }
    return 0;
}
