#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <iomanip>
using namespace std;
int listred[5] = {2,3,4,1,0};
int listblue[5] = {3,0,1,2,4};
int healthlist[5] = {};
const char * namelist[5] = {"dragon", "ninja", "iceman", "lion", "wolf"};
int sequence;
class soldier{
public:
    const char * name;
    int health, id;
    soldier(int number): name(namelist[number]),health(healthlist[number]) {}
};
class base{
    
    int next = 0, mini;
    int eachnum[2] = {0};
    int snum[5] = {0};
public:
    int total_life;
    const char* basename;
    vector<int> soldiers;
    base (const char* name, int m, int min): total_life(m), basename(name),  mini(min){}
    void Create(int t){
        int nextone;
        if (basename == "red")  {nextone = listred[next]; sequence = 0;}
        if (basename == "blue")  {nextone = listblue[next]; sequence = 1;}
        if (total_life < healthlist[nextone]) {
			next = (++next) % 5;
			Create(t);
			return;
		}
        soldier s(nextone);
        snum[nextone]++;
        eachnum[sequence]++;
        total_life -= s.health;
        cout << setw(3) << setfill('0') << t << " " << basename << " " << s.name << " "<< eachnum[sequence] << " ";
        cout << "born with strength " << s.health << ',' << snum[nextone] << " " << s.name << " ";
        cout << "in " << basename << " headquarter" << endl;
        next = (++next) % 5;
    }
    void Print(){

    }
};
int main(){
    int num;
    cin >> num;
    for (int i = 1;i <= num;i++){
        int time = 0;

        int m;
        cin >> m;
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
