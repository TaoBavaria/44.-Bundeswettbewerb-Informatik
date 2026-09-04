/*
Bundeswettbewerb Informatik 2025 Runde 2 Aufgabe 3
Autor: Tao Zheng
*/

#include <bits/stdc++.h>
using namespace std;

int LOG2(int a) {
    // Logarithmus zur Basis 2
    return 31 - __builtin_clz(a);
}

void getBackupTrack(vector<vector<int>>& paths, vector<vector<int>>& backtrack, vector<int>& rev_ids, int mask, int start, int N, int backup) {
    int pos = start;
    int nextPos = backup;
    // Strecke vom geplanten Knoten zum Ersatzknoten bestimmen
    while(pos != nextPos) { // Solange der aktuelle Knoten nicht der Ersatzknoten ist -> Zum Ersatzknoten nähern
        if (pos == 0) {
            cout << "S" << " ";
        } else if (pos == N - 1) {
            cout << "T" << " ";
        } else {
            cout << pos << " ";
        }
        pos = backtrack[nextPos][pos];
    }
    cout << "[" << pos << " " << rev_ids[pos] << "] ";
    
    // Vollständige Strecke vom Ersatzknoten zum Endknoten bestimmen.
    nextPos = paths[mask][pos];
    mask = mask & (~(+1 << (rev_ids[pos] - 1))); // Arbeitsschritt als fertig markieren.
    while (mask || pos != N - 1) { // Solange man nicht beim Endknoten angekommen ist.
        pos = backtrack[nextPos][pos];
        while(pos != nextPos) { // Solange man nicht beim nächsten Fabrik angekommen ist
            if (pos == 0) {
                cout << "S" << " ";
            } else if (pos == N - 1) {
                cout << "T" << " ";
            } else {
                cout << pos << " ";
            }
            pos = backtrack[nextPos][pos];
        }
        // Sobald man den nächsten Arbeitsschritt durchführen kann / Sobald man beim Endknoten angekommen ist.
        if (nextPos == N - 1) {
            cout << "T" << "\n";
        } else {
            cout << "[" << pos << " " << rev_ids[pos] << "] ";
            nextPos = paths[mask][pos];
            mask = mask & (~(+1 << (rev_ids[pos] - 1))); // Arbeitsschritt als fertig markieren.
        }
    }
}

void getTrack(vector<vector<int>>& paths, vector<vector<int>>& backtrack, vector<int>& rev_ids, int mask, int start, int N) {
    // Vollständige Strecke vom Startknoten zum Endknoten bestimmen
    cout << "S" << " ";
    int pos = start;
    int nextPos = paths[mask][pos];
    while (mask || pos != N - 1) { // Solange man nicht beim Endknoten angekommen ist.
        pos = backtrack[nextPos][pos];
        while(pos != nextPos) { // Solange man nicht beim nächsten Fabrik angekommen ist
            if (pos == 0) {
                cout << "S" << " ";
            } else if (pos == N - 1) {
                cout << "T" << " ";
            } else {
                cout << pos << " ";
            }
            pos = backtrack[nextPos][pos];
        }
        // Sobald man den nächsten Arbeitsschritt durchführen kann / Sobald man beim Endknoten angekommen ist.
        if (nextPos == N - 1) {
            cout << "T" << "\n";
        } else {
            cout << "[" << pos << " " << rev_ids[pos] << "] ";
            mask = mask & (~(+1 << (rev_ids[pos] - 1))); // Arbeitsschritt als fertig markieren.
            nextPos = paths[mask][pos];
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
    vector<vector<int>> ids(tasks + 2); // Arbeitschritte -> Fabriken
    vector<int> rev_ids(N); // Fabriken -> Arbeitschritte
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
    
    // dial's algorithm (Code abgeändert aus https://www.geeksforgeeks.org/dsa/dials-algorithm-optimized-dijkstra-for-small-range-weights/)
    
    vector<vector<int>> backtrack(N, vector<int>(N, -1));
    vector<vector<int>> dist(N, vector<int>(N, 1e9));
    int maxDist = (N - 1) * maxWeight;
    vector<vector<int>> buckets(maxDist + 1);
    
    for (int i = 0; i < N; i++) {
        dist[i][i] = 0;
        backtrack[i][i] = i;
        buckets[0].push_back(i);
        int highDist = 0;
        for (int d = 0; d <= maxDist && d <= highDist; d++) {
            while (!buckets[d].empty()) {
                int u = buckets[d].back();
                buckets[d].pop_back();

                if (d > dist[i][u]) continue;
                for (auto& [v, weight] : adj[u]) {
                    int newDist = dist[i][u] + weight;

                    if (newDist < dist[i][v]) {
                        dist[i][v] = newDist;
                        backtrack[i][v] = u;
                        buckets[newDist].push_back(v);
                        highDist = max(highDist, newDist);
                    }
                }
            }
        }
    }

    // Für jede Werkstatt eine Alternativwerkstatt
    vector<vector<int>> backups(1 << (tasks + 1), vector<int>(N, -1));
    // Kürzester Weg vom aktuellen Knoten zum Endknoten (unter Beachtung von Werkstätten, die noch besucht werden müssen)
    vector<vector<int>> defaultPaths(1 << (tasks + 1),  vector<int>(N, -1));
    // Der beste Weg für den Fall, dass keine Werke streiken.
    vector<vector<int>> recommendedPaths(1 << (tasks + 1), vector<int>(N, -1));

    vector<vector<vector<int>>> DP(1 << (tasks + 1), vector<vector<int>>(N, vector<int>(4, 1e9)));

    // Zielknoten -> 0 Distanz
    for (int u : ids[tasks + 1]) {
        DP[0b0][u][3] = 0;
        DP[0b0][u][2] = 0;
        DP[0b0][u][1] = 0;
        DP[0b0][u][0] = 0;
        defaultPaths[0b0][N - 1] = N - 1;
        for (int i = 1; i < N - 1; i++) {
            if (u == i) continue;
            DP[1 << (rev_ids[i] - 1)][i][0] = dist[u][i];
            defaultPaths[1 << (rev_ids[i] - 1)][i] = N - 1;
        }
    }
    
    // Kürzeste Wege vom Endknoten berechnen
    for (int mask = 0; mask < (1 << (tasks)); mask++) {
        for (int task = 0; task <= tasks; task++) {
            if (mask & (1 << task)) continue;
            int counter = mask;
            while (counter) {
                int startWorkstation = LOG2(counter & -counter) + 1; // Letzte besuchte Werk
                counter -= counter & -counter;
                for (int u : ids[startWorkstation]) {
                    for (int v : ids[task + 1]) { // Alle Werke mit noch nicht durchgeführten Arbeitsschritten besuchen.
                        if (DP[mask][u][0] + dist[u][v] < DP[mask | (1 << task)][v][0]) {
                            DP[mask | (1 << task)][v][0] = DP[mask][u][0] + dist[u][v];
                            defaultPaths[mask | (1 << task)][v] = u;
                        }
                    }
                }

            }
        }

        // Kürzeste Umwege zu einem Alternativwerk bestimmen
        for (int task = 0; task <= tasks; task++) {
            if (!(mask & (1 << task))) continue;
            for (int u : ids[task + 1]) {
                for (int v : ids[task + 1]) {
                    if (u == v) continue;
                    if (DP[mask][v][0] + dist[u][v] < DP[mask][u][1]) {
                        DP[mask][u][1] = DP[mask][v][0] + dist[u][v];
                        backups[mask][u] = v;
                    }
                }
            }
        }
    }

    // Empfohlenen Weg bestimmen
    for (int mask = 0; mask < (1 << tasks); mask++) {
        // Beim ersten Mal durchführen: Alle möglichen Wege aus dem Endknoten berechnen. 
        if (mask == 0) {
            for (int task = 0; task <= tasks; task++) {
                for (int u : ids[task + 1]) {
                    for (int v : ids[tasks + 1]) {
                        if (DP[mask][v][2] + dist[v][u] < DP[mask | (1 << task)][u][2]) {
                            DP[mask | (1 << task)][u][2] = DP[mask][v][2] + dist[v][u];
                            DP[mask | (1 << task)][u][3] = DP[mask][v][3] + dist[v][u];
                            recommendedPaths[mask][u] = v;
                        }
                    }
                    // Wenn das aktuelle Alternativwerk länger dauert als alle zuvorige Alternativwerke
                    if (DP[mask | (1 << task)][u][2] < DP[mask | (1 << task)][u][1]) {
                        DP[mask | (1 << task)][u][2] = DP[mask | (1 << task)][u][1];
                    }
                }
            }
        }

        // Wenn das aktuelle Alternativwerk länger dauert als alle zuvorige Alternativwerke
        {
            int counter = mask;
            while (counter) {
                int startWorkstation = LOG2(counter & -counter) + 1;
                counter -= counter & -counter;
                for (int u : ids[startWorkstation]) {
                    DP[mask][u][2] = max(DP[mask][u][2], DP[mask][u][1]);
                }
            }
        }

        // Aus allen besuchten Werke -> Nächste Werke, dessen Arbeitsschritt noch nicht durchgeführt wurde.
        for (int task = 0; task <= tasks; task++) {
            if (mask & (1 << task)) continue;
            int counter = mask;
            while (counter) {
                int startWorkstation = LOG2(counter & -counter) + 1;
                counter -= counter & -counter;
                for (int u : ids[task + 1]) {
                    for (int v : ids[startWorkstation]) {
                        if (DP[mask][v][2] + dist[v][u] < DP[mask | (1 << task)][u][2]) {
                            DP[mask | (1 << task)][u][2] = DP[mask][v][2] + dist[v][u];
                            DP[mask | (1 << task)][u][3] = DP[mask][v][3] + dist[v][u];
                            recommendedPaths[mask][u] = v;
                        }
                    }
                }
            }
        }
    }

    // Aus allen Werken (unter der Voraussetzung, dass alle Arbeitsschritte durchgeführt wurden) den kürzesten Weg zum Startknoten berechnen.
    int mask = (1 << (tasks)) - 1;
    for (int task = 0; task <= tasks; task++) {
        for(int u : ids[task + 1]) {
            if (DP[mask][u][2] + dist[u][0] < DP[mask][0][2]) {
                DP[mask][0][2] = DP[mask][u][2] + dist[u][0];
                DP[mask][0][3] = DP[mask][u][3] + dist[u][0];
                recommendedPaths[mask][0] = u;
            }
        }
    }
    
    // Falls der Betrag der worst case Streckenlänge das Zeitlimit überschreiten sollte. -> Kein Weg möglich
    if (DP[(1 << (tasks)) - 1][0][2] > min(timelimit, (int) 1e9 - 1)) {
        cout << "UNMOEGLICH" << endl;
        return;
    }
    cout << "MOEGLICH" << "\n";
    cout << DP[mask][0][2] << "\n";
    cout << "\n";
    cout << DP[mask][0][3] << "\n";
    getTrack(recommendedPaths, backtrack, rev_ids, mask, 0, N); // Empfohlene Strecke ausgeben
    int countmask = mask;
    int pos = 0;
    // Alle Fällebearbeitung, falls einzelne Werke geschlossen sind.
    while (countmask) {
        pos = recommendedPaths[countmask][pos];
        cout << DP[mask][0][3] - DP[countmask][pos][3] + DP[countmask][pos][1] << "\n"; // Gesamtlänge der Alternativstrecke berechnen
        getBackupTrack(defaultPaths, backtrack, rev_ids, countmask, pos, N, backups[countmask][pos]); // Alternativstrecke ausgeben.
        countmask = countmask & (~(1 << (rev_ids[pos] - 1))); // Arbeitsschritt als fertig markieren.
    }
    cout << flush;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    vector<string> filenames = {
        ///"lieferung00.txt",
        ///"lieferung01.txt",
        ///"lieferung02.txt",
        ///"lieferung03.txt",
        "lieferung04.txt",
        //"lieferung05.txt",
        //"lieferung06.txt",
        //"lieferung07.txt",
        //"lieferung08.txt",
        ///"lieferung09.txt",
        //"lieferung10.txt",
    };

    string filename = filenames[0];
    for (string filename : filenames) {
        string filein = "A3_Lieferkette\\" + filename;
        string fileout = "Ausgabe\\" + filename + "_out_bitmask.txt";
        freopen(filein.c_str(), "r", stdin);
        freopen(fileout.c_str(), "w", stdout);
        auto start = chrono::high_resolution_clock::now();
        solve();
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "\nExecution time: " << duration.count() << "μs" << endl;
    }
    
}