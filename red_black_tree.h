
#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <iostream>
using namespace std;

class RedBlackTree {

private:

    enum Couleur { ROUGE, NOIR };

    struct Noeud {
        int valeur;
        Couleur couleur;

        Noeud* gauche;
        Noeud* droite;
        Noeud* parent;

        Noeud(int v, Couleur c)
            : valeur(v), couleur(c),
              gauche(nullptr), droite(nullptr), parent(nullptr) {}
    };

    Noeud* racine;
    Noeud* NIL;
    int nbElements;

    // Recherche interne
    Noeud* rechercherNoeud(int valeur) const {

        Noeud* courant = racine;

        while (courant != NIL) {

            if (valeur == courant->valeur)
                return courant;

            if (valeur < courant->valeur)
                courant = courant->gauche;
            else
                courant = courant->droite;
        }

        return NIL;
    }

    // Rotation gauche
    void rotationGauche(Noeud* x) {

        Noeud* y = x->droite;

        x->droite = y->gauche;

        if (y->gauche != NIL)
            y->gauche->parent = x;

        y->parent = x->parent;

        if (x->parent == NIL)
            racine = y;
        else if (x == x->parent->gauche)
            x->parent->gauche = y;
        else
            x->parent->droite = y;

        y->gauche = x;
        x->parent = y;
    }

    // Rotation droite
    void rotationDroite(Noeud* y) {

        Noeud* x = y->gauche;

        y->gauche = x->droite;

        if (x->droite != NIL)
            x->droite->parent = y;

        x->parent = y->parent;

        if (y->parent == NIL)
            racine = x;
        else if (y == y->parent->droite)
            y->parent->droite = x;
        else
            y->parent->gauche = x;

        x->droite = y;
        y->parent = x;
    }

    // Correction apres insertion
    void corrigerInsertion(Noeud* z) {

        while (z->parent->couleur == ROUGE) {

            if (z->parent == z->parent->parent->gauche) {

                Noeud* oncle = z->parent->parent->droite;

                // Cas 1 : oncle rouge
                if (oncle->couleur == ROUGE) {

                    z->parent->couleur = NOIR;
                    oncle->couleur = NOIR;
                    z->parent->parent->couleur = ROUGE;

                    z = z->parent->parent;
                }

                else {

                    // Cas 2 : triangle
                    if (z == z->parent->droite) {
                        z = z->parent;
                        rotationGauche(z);
                    }

                    // Cas 3 : ligne
                    z->parent->couleur = NOIR;
                    z->parent->parent->couleur = ROUGE;

                    rotationDroite(z->parent->parent);
                }
            }

            else {

                Noeud* oncle = z->parent->parent->gauche;

                // Cas 1 : oncle rouge
                if (oncle->couleur == ROUGE) {

                    z->parent->couleur = NOIR;
                    oncle->couleur = NOIR;
                    z->parent->parent->couleur = ROUGE;

                    z = z->parent->parent;
                }

                else {

                    // Cas 2 : triangle
                    if (z == z->parent->gauche) {
                        z = z->parent;
                        rotationDroite(z);
                    }

                    // Cas 3 : ligne
                    z->parent->couleur = NOIR;
                    z->parent->parent->couleur = ROUGE;

                    rotationGauche(z->parent->parent);
                }
            }
        }

        racine->couleur = NOIR;
    }

    // Minimum
    Noeud* minimum(Noeud* node) const {

        while (node->gauche != NIL)
            node = node->gauche;

        return node;
    }

    // Remplacement d'un sous-arbre
    void transplanter(Noeud* u, Noeud* v) {

        if (u->parent == NIL)
            racine = v;
        else if (u == u->parent->gauche)
            u->parent->gauche = v;
        else
            u->parent->droite = v;

        v->parent = u->parent;
    }

    // Correction apres suppression
    void corrigerSuppression(Noeud* x) {

        while (x != racine && x->couleur == NOIR) {

            if (x == x->parent->gauche) {

                Noeud* frere = x->parent->droite;

                // Cas 1
                if (frere->couleur == ROUGE) {

                    frere->couleur = NOIR;
                    x->parent->couleur = ROUGE;

                    rotationGauche(x->parent);
                    frere = x->parent->droite;
                }

                // Cas 2
                if (frere->gauche->couleur == NOIR &&
                    frere->droite->couleur == NOIR) {

                    if (frere != NIL)
                        frere->couleur = ROUGE;

                    x = x->parent;
                }

                else {

                    // Cas 3
                    if (frere->droite->couleur == NOIR) {

                        frere->gauche->couleur = NOIR;
                        frere->couleur = ROUGE;

                        rotationDroite(frere);
                        frere = x->parent->droite;
                    }

                    // Cas 4
                    frere->couleur = x->parent->couleur;
                    x->parent->couleur = NOIR;
                    frere->droite->couleur = NOIR;

                    rotationGauche(x->parent);
                    x = racine;
                }
            }

            else {

                Noeud* frere = x->parent->gauche;

                // Cas 1
                if (frere->couleur == ROUGE) {

                    frere->couleur = NOIR;
                    x->parent->couleur = ROUGE;

                    rotationDroite(x->parent);
                    frere = x->parent->gauche;
                }

                // Cas 2
                if (frere->droite->couleur == NOIR &&
                    frere->gauche->couleur == NOIR) {

                    if (frere != NIL)
                        frere->couleur = ROUGE;

                    x = x->parent;
                }

                else {

                    // Cas 3
                    if (frere->gauche->couleur == NOIR) {

                        frere->droite->couleur = NOIR;
                        frere->couleur = ROUGE;

                        rotationGauche(frere);
                        frere = x->parent->gauche;
                    }

                    // Cas 4
                    frere->couleur = x->parent->couleur;
                    x->parent->couleur = NOIR;
                    frere->gauche->couleur = NOIR;

                    rotationDroite(x->parent);
                    x = racine;
                }
            }
        }

        x->couleur = NOIR;
    }

    // Parcours infixe
    void parcoursInfixe(Noeud* node) const {

        if (node != NIL) {

            parcoursInfixe(node->gauche);

            cout << node->valeur
                 << "(" << (node->couleur == ROUGE ? "R" : "N") << ") ";

            parcoursInfixe(node->droite);
        }
    }

    // Liberation de la memoire
    void detruire(Noeud* node) {

        if (node != NIL) {
            detruire(node->gauche);
            detruire(node->droite);
            delete node;
        }
    }

public:

    // Constructeur
    RedBlackTree() {

        NIL = new Noeud(0, NOIR);

        NIL->gauche = NIL;
        NIL->droite = NIL;
        NIL->parent = NIL;

        racine = NIL;
        nbElements = 0;
    }

    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;

    // Destructeur
    ~RedBlackTree() {

        detruire(racine);
        delete NIL;
    }

    // Insertion
    void inserer(int valeur) {

        if (rechercherNoeud(valeur) != NIL)
            return;

        Noeud* z = new Noeud(valeur, ROUGE);

        z->gauche = NIL;
        z->droite = NIL;

        Noeud* y = NIL;
        Noeud* x = racine;

        while (x != NIL) {

            y = x;

            if (z->valeur < x->valeur)
                x = x->gauche;
            else
                x = x->droite;
        }

        z->parent = y;

        if (y == NIL)
            racine = z;
        else if (z->valeur < y->valeur)
            y->gauche = z;
        else
            y->droite = z;

        nbElements++;

        corrigerInsertion(z);
    }

    // Recherche
    bool rechercher(int valeur) const {
        return rechercherNoeud(valeur) != NIL;
    }

    // Suppression
    void supprimer(int valeur) {

        Noeud* z = rechercherNoeud(valeur);

        if (z == NIL)
            return;

        Noeud* y = z;
        Noeud* x;

        Couleur ancienneCouleur = y->couleur;

        if (z->gauche == NIL) {

            x = z->droite;
            transplanter(z, z->droite);
        }

        else if (z->droite == NIL) {

            x = z->gauche;
            transplanter(z, z->gauche);
        }

        else {

            y = minimum(z->droite);
            ancienneCouleur = y->couleur;

            x = y->droite;

            if (y->parent == z)
                x->parent = y;

            else {

                transplanter(y, y->droite);

                y->droite = z->droite;
                y->droite->parent = y;
            }

            transplanter(z, y);

            y->gauche = z->gauche;
            y->gauche->parent = y;

            y->couleur = z->couleur;
        }

        delete z;
        nbElements--;

        if (ancienneCouleur == NOIR)
            corrigerSuppression(x);

        NIL->couleur = NOIR;
        NIL->parent = NIL;
    }

    // Parcours public
    void parcoursInfixe() const {
        parcoursInfixe(racine);
        cout << endl;
    }

    // Taille
    int taille() const {
        return nbElements;
    }

    // Arbre vide
    bool estVide() const {
        return racine == NIL;
    }
};

#endif
