#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <random>
#include <string>
#include <vector>

#include "red_black_tree.h"

using namespace std;
using namespace chrono;

const int N = 100000;
const int REP_RECHERCHE = 100000;   // repetitions pour moyenner une recherche (trop rapide pour une seule mesure)
const int REP_SUPPRESSION = 1000;   // repetitions pour moyenner une suppression

volatile long long sink = 0;        // empeche le compilateur de supprimer les recherches "inutiles"

// ---------------------------------------------------------------
// Outils
// ---------------------------------------------------------------
template <typename Fonction>
double mesurerTemps(Fonction operation) {   // retourne des millisecondes
    auto debut = high_resolution_clock::now();
    operation();
    auto fin = high_resolution_clock::now();
    return duration<double, milli>(fin - debut).count();
}

struct Jeu {
    string nom;
    vector<int> donnees;
};

vector<int> genererAleatoire() {
    vector<int> v(N);
    iota(v.begin(), v.end(), 0);
    mt19937 gen(42);
    shuffle(v.begin(), v.end(), gen);
    return v;
}

vector<int> genererTrie() {
    vector<int> v(N);
    iota(v.begin(), v.end(), 0);
    return v;
}

vector<int> genererPresqueTrie() {
    vector<int> v(N);
    iota(v.begin(), v.end(), 0);
    mt19937 gen(42);
    uniform_int_distribution<int> dist(0, N - 2);
    for (int i = 0; i < N / 100; ++i) {
        int idx = dist(gen);
        swap(v[idx], v[idx + 1]);
    }
    return v;
}

int lireEntier(const string& message) {
    int x;
    cout << message;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entree invalide. " << message;
    }
    return x;
}

void ligne(const vector<int>& largeurs) {
    cout << "+";
    for (int l : largeurs) cout << string(l + 2, '-') << "+";
    cout << "\n";
}

void rangee(const vector<int>& largeurs, const vector<string>& cellules) {
    cout << "|";
    for (size_t i = 0; i < cellules.size(); ++i)
        cout << " " << left << setw(largeurs[i]) << cellules[i] << " |";
    cout << "\n";
}

string fmt(double x, int precision = 2) {
    ostringstream os;
    os << fixed << setprecision(precision) << x;
    return os.str();
}

// ---------------------------------------------------------------
// 1) INSERTION : un tableau, une ligne par jeu de donnees
// ---------------------------------------------------------------
void tableauInsertion(const vector<Jeu>& jeux) {
    vector<int> w = {26, 20, 20, 16};
    cout << "\n=== INSERTION de " << N << " elements ===\n";
    ligne(w);
    rangee(w, {"Jeu de donnees", "Red-Black Tree (ms)", "std::map (ms)", "Ratio RB/map"});
    ligne(w);
    for (const Jeu& j : jeux) {
        double tRB, tMap;
        {
            RedBlackTree arbre;
            tRB = mesurerTemps([&]() { for (int x : j.donnees) arbre.inserer(x); });
        }
        {
            map<int, int> m;
            tMap = mesurerTemps([&]() { for (int x : j.donnees) m.insert({x, 0}); });
        }
        rangee(w, {j.nom, fmt(tRB), fmt(tMap), fmt(tRB / tMap)});
    }
    ligne(w);
    cout << "(ratio > 1 : le Red-Black Tree est plus lent que std::map)\n";
}

// ---------------------------------------------------------------
// 2) RECHERCHE : l'utilisateur saisit la valeur
// ---------------------------------------------------------------
void tableauRecherche(const vector<Jeu>& jeux) {
    int v = lireEntier("Valeur a rechercher : ");

    vector<int> w = {26, 8, 18, 8, 18};
    cout << "\n=== RECHERCHE de la valeur " << v << " (temps moyen sur " << REP_RECHERCHE << " essais) ===\n";
    ligne(w);
    rangee(w, {"Jeu de donnees", "RB", "RB temps (ns)", "map", "map temps (ns)"});
    ligne(w);
    for (const Jeu& j : jeux) {
        RedBlackTree rb;
        map<int, int> m;
        for (int x : j.donnees) { rb.inserer(x); m.insert({x, 0}); }   // construction non mesuree

        bool trouveRB  = static_cast<bool>(rb.rechercher(v));
        bool trouveMap = (m.find(v) != m.end());

        double msRB = mesurerTemps([&]() {
            long long c = 0;
            for (int i = 0; i < REP_RECHERCHE; ++i) c += static_cast<bool>(rb.rechercher(v));
            sink = sink + c;
        });
        double msMap = mesurerTemps([&]() {
            long long c = 0;
            for (int i = 0; i < REP_RECHERCHE; ++i) c += (m.find(v) != m.end());
            sink = sink + c;
        });

        rangee(w, {j.nom, trouveRB ? "oui" : "non", fmt(msRB * 1e6 / REP_RECHERCHE, 1),
                   trouveMap ? "oui" : "non", fmt(msMap * 1e6 / REP_RECHERCHE, 1)});
    }
    ligne(w);
}

// ---------------------------------------------------------------
// 3) SUPPRESSION : l'utilisateur saisit la valeur
// ---------------------------------------------------------------
// Mesure le temps moyen d'une suppression : chaque suppression est chronometree seule,
// puis la valeur est reinseree (hors mesure) pour pouvoir recommencer.
template <typename Supprimer, typename Reinserer>
double moyenneSuppressionNs(Supprimer supp, Reinserer rein, bool present) {
    double total = 0;
    for (int i = 0; i < REP_SUPPRESSION; ++i) {
        auto debut = high_resolution_clock::now();
        supp();
        auto fin = high_resolution_clock::now();
        total += duration<double, nano>(fin - debut).count();
        if (present) rein();
    }
    return total / REP_SUPPRESSION;
}

void tableauSuppression(const vector<Jeu>& jeux) {
    int v = lireEntier("Valeur a supprimer : ");

    vector<int> w = {26, 8, 18, 18, 11, 11};
    cout << "\n=== SUPPRESSION de la valeur " << v << " (temps moyen sur " << REP_SUPPRESSION << " essais) ===\n";
    ligne(w);
    rangee(w, {"Jeu de donnees", "Present", "RB temps (ns)", "map temps (ns)", "Taille RB", "Taille map"});
    ligne(w);
    for (const Jeu& j : jeux) {
        RedBlackTree rb;
        map<int, int> m;
        for (int x : j.donnees) { rb.inserer(x); m.insert({x, 0}); }

        bool present = (m.find(v) != m.end());

        double nsRB  = moyenneSuppressionNs([&]() { rb.supprimer(v); }, [&]() { rb.inserer(v); }, present);
        double nsMap = moyenneSuppressionNs([&]() { m.erase(v); }, [&]() { m.insert({v, 0}); }, present);

        // suppression "reelle" finale pour afficher la taille apres
        rb.supprimer(v);
        m.erase(v);

        rangee(w, {j.nom, present ? "oui" : "non", fmt(nsRB, 1), fmt(nsMap, 1),
                   to_string(rb.taille()), to_string(m.size())});
    }
    ligne(w);
    cout << "(les valeurs presentes dans les arbres vont de 0 a " << N - 1 << ")\n";
}

// ---------------------------------------------------------------
// Programme : insertion -> recherche (valeur saisie) -> suppression (valeur saisie)
// ---------------------------------------------------------------
int main() {
    vector<Jeu> jeux = {
        {"Aleatoires",     genererAleatoire()},
        {"Triees",         genererTrie()},
        {"Presque triees", genererPresqueTrie()},
    };

    tableauInsertion(jeux);
    tableauRecherche(jeux);
    tableauSuppression(jeux);

    return 0;
}