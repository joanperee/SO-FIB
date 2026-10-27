#include <iostream>
#include <vector>
#include <queue>
#include <tuple>

using namespace std;

// Direccions possibles: amunt, avall, esquerra, dreta
const vector<pair<int, int>> direccions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

// Funció principal
int main() {
    int n, m;
    cin >> n >> m;

    vector<string> mapa(n);
    for (int i = 0; i < n; ++i) {
        cin >> mapa[i];
    }

    int f, c;
    cin >> f >> c;

    // Ajustem les coordenades per començar des de l'índex 0 (fila, columna)
    f -= 1;
    c -= 1;

    // Inicialitzem la cua per a la BFS
    queue<tuple<int, int, int>> q;
    q.push({f, c, 0});

    vector<vector<bool>> visitat(n, vector<bool>(m, false));
    visitat[f][c] = true;

    while (!q.empty()) {
        auto [x, y, dist] = q.front();
        q.pop();

        // Si hem trobat un tresor, imprimim la distància i sortim
        if (mapa[x][y] == 't') {
            cout << "distancia minima: " << dist << endl;
            return 0;
        }

        // Explorem les quatre direccions
        for (auto [dx, dy] : direccions) {
            int nx = x + dx;
            int ny = y + dy;

            // Verifiquem que estem dins dels límits del mapa i en una posició accessible
            if (nx >= 0 && nx < n && ny >= 0 && ny < m && !visitat[nx][ny] && mapa[nx][ny] != 'X') {
                visitat[nx][ny] = true;
                q.push({nx, ny, dist + 1});
            }
        }
    }

    // Si hem explorat tot i no hem trobat cap tresor
    cout << "no es pot arribar a cap tresor" << endl;
    return 0;
}