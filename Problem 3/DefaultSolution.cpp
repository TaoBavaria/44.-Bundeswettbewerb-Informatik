/*
Bundeswettbewerb Informatik 2025 Runde 2 Aufgabe 3 Erweiterung
Autor: Tao Zheng
*/

#include <bits/stdc++.h>
using namespace std;

#define FOR(i,a) for (int i = 0; i < a; i++)
#define DEBUG(a) cout << "Debug: "<< #a << ": " << (a) << "\n"
#define MULTDEBUG(a) cout << #a << ": " << (a) << ", "

#define DEBUGVECTOR(a) cout << "DEBUGVECTOR: " << #a << "\n"; FOR(DEBUGVEC, a.size()) {cout << DEBUGVEC << ": " << a[DEBUGVEC] << "\n";} cout << "DEBUGVECTOR END";

#define DEBUGVV(a) cout << "DEBUGVV: " << #a << "\n"; \
    FOR(OUT, a.size()) { \
        cout << OUT << ": "; \
        FOR(IN, a[OUT].size()) { \
            cout << a[OUT][IN] << " "; \
        } \
        cout << "\n"; \
    } \
    cout << "DEBUGVV END" << "\n";

void fail() {
    cout << "UNMOEGLICH" << endl;
    return;
}

void getBackupTrack(vector<vector<int>>& backtrack, vector<string>& backup, int task) {
    // Strecke vom geplanten Knoten zum Ersatzknoten bestimmen
    for (int i = 0; i < backup.size() - 1; i++) {
        cout << backup[i] << " ";
    }
    cout << "[" << backup.back() << " " << task << "]" << " ";
    task++;
    
    // Vollständige Strecke vom Ersatzknoten zum Endknoten bestimmen
    int pointer = backtrack[task][stoi(backup.back())];
    int N = backtrack[0].size();
    int tasks = backtrack.size() - 2;
    while (task < tasks + 2) { // Solange man nicht beim Endknoten angekommen ist
        while(backtrack[task][pointer] != -1) { // Solange man nicht beim nächsten Fabrik angekommen ist
            if (pointer == 0) {
                cout << "S" << " ";
            } else if (pointer == N - 1) {
                cout << "T" << " ";
            } else {
                cout << pointer << " ";
            }
            pointer = backtrack[task][pointer];
        }
        if (task == tasks + 1) {
            cout << "T" << "\n";
            return;
        } else {
            cout << "[" << pointer << " " << task << "] " << flush;
            task++;
            pointer = backtrack[task][pointer];
        }
    }
}

void getTrack(vector<vector<int>>& backtrack) {
    // Vollständige Strecke vom Startknoten zum Endknoten bestimmen
    int task = 1;
    int pointer = 0;
    int N = backtrack[0].size();
    int tasks = backtrack.size() - 2;
    while (task < tasks + 2) { // Solange man nicht beim Endknoten angekommen ist
        while(backtrack[task][pointer] != -1) { // Solange man nicht beim nächsten Fabrik angekommen ist
            if (pointer == 0) {
                cout << "S" << " ";
            } else if (pointer == N - 1) {
                cout << "T" << " ";
            } else {
                cout << pointer << " ";
            }
            pointer = backtrack[task][pointer];
        }
        if (task == tasks + 1) {
            cout << "T" << "\n";
            return;
        } else {
            cout << "[" << pointer << " " << task << "] " << flush;
            task++;
            pointer = backtrack[task][pointer];
        }
    }
}



void solve() {
    int timelimit;
    cin >> timelimit; 
    int tasks;
    cin >> tasks;

    // redundanz begin
    for (int i = 0; i < tasks; i++) {
        int a;
        cin >> a;
    }
    // redundanz ende
    int w;
    cin >> w;
    int N = w + 2;
    vector<vector<int>> ids(tasks + 2);
    vector<int> rev_ids(N);
    ids[0].push_back(0); // Startknoten
    rev_ids[0] = 0;
    ids[tasks + 1].push_back(N - 1); // Endknoten
    rev_ids[N - 1] = tasks + 1;
    for (int i = 0; i < w; i++) {
        int a, b;
        cin >> a >> b;
        ids[b].push_back(a);
        rev_ids[a] = b;
    }

    int M;
    cin >> M;
    vector<vector<pair<int, int>>> adj(N);
    int maxWeight = 0;
    for (int i = 0; i < M; i++) {
        string a, b;
        int c;
        cin >> a >> b >> c;
        int na, nb;
        if (a == "S") {
            na = 0;
        } else if (a == "T") {
            na = N - 1;
        } else {
            na = stoi(a);
        }
        if (b == "S") {
            nb = 0;
        } else if (b == "T") {
            nb = N - 1;
        } else {
            nb = stoi(b);
        }
        adj[na].emplace_back(nb, c);
        adj[nb].emplace_back(na, c);
        maxWeight = max(maxWeight, c);
    }

    vector<vector<int>> DP(N, vector<int>(4, 1e9));
    /*
        DP States:
        DP[v][0]: Das Betrag der kürzeste Weg von Knoten v zur Endknoten unter Beachtung von Werkstätten, die noch besucht werden müssen.
        DP[v][1]: Das Betrag der kürzeste Weg von Knoten v zur Endknoten über einen Alternativwerkstatt, unter Beachtung von Werkstätten die noch besucht werden müssen.
            Der Weg wird wie gefolgt definiert: DP[v][1] = min(dist(v, w) + DP[w][0]) wobei w ein Werkstatt darstellt, welches die gleiche Arbeitsprozess wie v durchführen kann.
        DP[v][2]: Das Betrag der worst Case Weg von Knoten v zur Endknoten über alle kürzeste Ersatzwege.
            Der Weg wird wie gefolgt definiert: DP[v][2] = max(min(dist(v, w) + DP[w][2]), DP[v][1]) wobei w für ein Werkstatt darstellt, welches die darauf folgende Arbeitsprozess durchführen kann
        DP[v][3]: Der tatsächliche Betrag der länge des Weges, welches durch DP[v][2] bestimmt wurde, unter der Beachtung von Werkstätten, die noch besucht werden müssen.
    */

    // Zielknoten -> 0 Distanz
    for (int u : ids[tasks + 1]) {
        DP[u][3] = 0;
        DP[u][2] = 0;
        DP[u][1] = 0;
        DP[u][0] = 0;
    }

    // dial's algorithm (Code abgeändert aus https://www.geeksforgeeks.org/dsa/dials-algorithm-optimized-dijkstra-for-small-range-weights/)
    
    vector<vector<int>> dist(tasks + 2, vector<int>(N, 1e9));
    int maxDist = (N - 1) * maxWeight * 3;
    vector<vector<int>> buckets(maxDist + 1);

    // Verweist alle Knoten auf ihre Anfangsknoten
    vector<vector<int>> captures(tasks + 2, vector<int>(N, -1));

    // Kürzester Weg vom aktuellen Knoten zum Endknoten (unter Beachtung von Werkstätten, die noch besucht werden müssen) (Fall 0)
    vector<vector<int>> backtrack_Case0(tasks + 2, vector<int>(N, -1));
    // Für jede Werkstatt eine Alternativwerkstatt (Fall 1)
    vector<vector<string>> backtrack_Case1(N);
    // Der beste Weg für den Fall, dass keine Werke streiken. (Fall 2, 3)
    vector<int> recommendedPaths(N, -1);
    
    for (int task = tasks + 1; task > 0; task--) {
        // Vorbereitung Beginn
        int bias = 1e9;
        for (int id : ids[task]) {
            bias = min(bias, DP[id][0]);
        }
        int highDist = 0;
        for (int id : ids[task]) {
            dist[task][id] = DP[id][0];
            buckets[DP[id][0] - bias].push_back(id);
            highDist = max(highDist, DP[id][0] - bias);
            captures[task][id] = id;
        }
        // Vorbereitung Ende
        for (int d = 0; d <= maxDist && d <= highDist; d++) {
            while (!buckets[d].empty()) {
                int u = buckets[d].back();
                buckets[d].pop_back();

                if (d > dist[task][u] - bias) continue;
                for (auto& [v, weight] : adj[u]) {
                    int newDist = dist[task][u] + weight;

                    // Case 1 Bearbeitung
                    if (captures[task][v] != -1 && captures[task][v] != captures[task][u]) {
                        // Überprüfung Knoten v als Alternativknoten für Knoten u
                        if (dist[task][u] + weight - dist[task][captures[task][u]] + dist[task][v] < DP[captures[task][u]][1]) {
                            DP[captures[task][u]][1] = dist[task][u] + weight - dist[task][captures[task][u]] + dist[task][v];
                            // Backtrack Berechnung von u zur v.
                            vector<string> past;
                            int pointer = u;
                            while (pointer != -1) {
                                if (pointer == 0) {
                                    past.push_back("S");
                                } else if (pointer == N - 1) {
                                    past.push_back("T");
                                } else {
                                    past.push_back(to_string(pointer));
                                }
                                pointer = backtrack_Case0[task][pointer];
                            }
                            reverse(past.begin(), past.end());
                            pointer = v;
                            while (pointer != -1) {
                                if (pointer == 0) {
                                    past.push_back("S");
                                } else if (pointer == N - 1) {
                                    past.push_back("T");
                                } else {
                                    past.push_back(to_string(pointer));
                                }
                                pointer = backtrack_Case0[task][pointer];
                            }
                            backtrack_Case1[captures[task][u]] = past;
                        }
                        // Überprüfung Knoten u als Alternativknoten für Knoten v
                        if (dist[task][v] + weight - dist[task][captures[task][v]] + dist[task][u] < DP[captures[task][v]][1]) {
                            DP[captures[task][v]][1] = dist[task][v] + weight - dist[task][captures[task][v]] + dist[task][u];
                            // Backtrack Berechnung von v zur u.
                            vector<string> past;
                            int pointer = v;
                            while (pointer != -1) {
                                if (pointer == 0) {
                                    past.push_back("S");
                                } else if (pointer == N - 1) {
                                    past.push_back("T");
                                } else {
                                    past.push_back(to_string(pointer));
                                }
                                pointer = backtrack_Case0[task][pointer];
                            }
                            reverse(past.begin(), past.end());
                            pointer = u;
                            while (pointer != -1) {
                                if (pointer == 0) {
                                    past.push_back("S");
                                } else if (pointer == N - 1) {
                                    past.push_back("T");
                                } else {
                                    past.push_back(to_string(pointer));
                                }
                                pointer = backtrack_Case0[task][pointer];
                            }
                            backtrack_Case1[captures[task][v]] = past;
                        }
                    }

                    // Überprüfung neuer kürzester Knoten
                    if (newDist < dist[task][v]) {
                        dist[task][v] = newDist;
                        backtrack_Case0[task][v] = u;
                        buckets[newDist - bias].push_back(v);
                        highDist = max(highDist, newDist);
                        captures[task][v] = captures[task][u];
                        // Case 0 Bearbeitung
                        if (rev_ids[v] == task - 1) {
                            if (newDist < DP[v][0]) {
                                DP[v][0] = newDist;
                            }
                        }
                    }
                }
            }
        }
    }

    // Separate Trennung der Variablen für den zweiten Dial's Algorithmus Aktivierung
    vector<vector<int>> dist_case2(tasks + 2, vector<int>(N, 1e9));
    vector<vector<int>> captures_case2(tasks + 2, vector<int>(N, -1));
    // Empfohlener Weg vom Startknoten S zum Endknoten T unter Beachtung der Arbeitsschritte. (Fall 2 und 3)
    vector<vector<int>> backtrack_Case2(tasks + 2, vector<int>(N, -1));

    for (int task = tasks + 1; task > 0; task--) {
        // Vorbereitung Beginn
        int bias = 1e9;
        for(int id : ids[task]) {
            bias = min(bias, DP[id][2]);
        }

        int highDist = 0;
        for (int id : ids[task]) {
            dist_case2[task][id] = DP[id][2];
            buckets[DP[id][2] - bias].push_back(id);
            highDist = max(highDist, DP[id][2] - bias);
            captures_case2[task][id] = id;
        }
        // Vorbereitung Ende
        for (int d = 0; d <= maxDist && d <= highDist; d++) {
            while (!buckets[d].empty()) {
                int u = buckets[d].back();
                buckets[d].pop_back();
                if (d > dist_case2[task][u] - bias) continue;
                for (auto& [v, weight] : adj[u]) {
                    int newDist = dist_case2[task][u] + weight;

                    if (newDist < dist_case2[task][v]) {
                        dist_case2[task][v] = newDist;
                        backtrack_Case2[task][v] = u;
                        buckets[newDist - bias].push_back(v);
                        highDist = max(highDist, newDist);
                        captures_case2[task][v] = captures_case2[task][u];
                        
                        // Überprüfung: Fall 2 und 3
                        if (rev_ids[v] == task - 1) {
                            if (newDist < DP[v][2]) {
                                DP[v][2] = newDist;
                                DP[v][3] = dist_case2[task][v] - dist_case2[task][captures_case2[task][u]] + DP[captures_case2[task][u]][3];
                                recommendedPaths[v] = captures_case2[task][u];
                            }
                        }
                    }
                }
            }
        }

        // Fall 2: Ist der aktuelle Alternativweg länger oder die Wege zu allen bisherigen Alternativwegen?
        if (task - 1 != 0) {
            for (auto id : ids[task - 1]) {
                if (DP[id][2] < DP[id][1]) {
                    DP[id][2] = DP[id][1];
                }
            }
        }
    }

    // Falls der Betrag der worst case Streckenlänge das Zeitlimit überschreiten sollte.
    if (DP[0][2] > min(timelimit, (int) 1e9 - 1)) {
        fail();
        return;
    }
    cout << "MOEGLICH" << "\n";
    // Worst-Case Dauer
    cout << DP[0][2] << "\n"; 
    cout << "\n";
    // empfohlener Weg
    cout << DP[0][3] << "\n"; 
    getTrack(backtrack_Case2);
    // Ausgabe aller Alternativwege
    int pos = recommendedPaths[0];
    int task = 1;
    while (pos != N - 1) {
        cout << DP[0][3] - DP[pos][3] + DP[pos][1] << "\n";
        getBackupTrack(backtrack_Case0, backtrack_Case1[pos], task); 
        task++;
        pos = recommendedPaths[pos];
    }
    cout << flush;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    vector<string> filenames = {
        "lieferung00.txt",
        "lieferung01.txt",
        "lieferung02.txt",
        "lieferung03.txt",
        "lieferung04.txt",
        "lieferung05.txt",
        "lieferung06.txt",
        "lieferung07.txt",
        "lieferung08.txt",
        "lieferung09.txt",
        "lieferung10.txt",
    };

    string filename = filenames[0];
    for (string filename : filenames) {
        string filein = "A3_Lieferkette\\" + filename;
        string fileout = "Ausgabe\\" + filename + "_out.txt";
        freopen(filein.c_str(), "r", stdin);
        freopen(fileout.c_str(), "w", stdout);
        //auto start = chrono::high_resolution_clock::now();
        solve();
        //auto end = chrono::high_resolution_clock::now();
        //auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        //freopen("CON", "w", stdout);
        //cout << filename << "\nExecution time: " << duration.count() << "μs" << endl;
    }
    
}