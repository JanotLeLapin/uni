---
title: Ensembles
---

## Description des Ensembles

Un ensemble est une collection d'objets (éléments).

### Exemples d'Ensembles

- $\mathbb N$ : nombres entiers positifs
- $\mathbb Z$ : nombres entiers relatifs
- $\mathbb Q$ : nombres rationels
- $\mathbb R$ : nombres réels

### Définitions

- Si $x$ est un élément d'un ensemble $A$, on dit $x \in A$. Sinon, on dit $x \notin A$.
- Un ensemble qui contient aucun élément est appelé ensemble vide. Il est souvent noté $\emptyset$.
- Un ensemble qui contient un seul élément est appelé un singleton.
- L'ordre n'a pas d'importance dans les ensembles.

#### Opérations sur les Ensembles

Soient $A$, $B$ et $E$ trois ensembles tels que $A \subset E$ :

On peut construire de nouveaux ensembles en définissant les opérations suivantes:

- $A \cap B$ : l'ensemble qui contient les éléments de $A$ **et** $B$ (intersection).
- $A \cup B$ : l'ensemble qui contient les éléments de $A$ **ou** $B$ (union).
- $A \setminus B$ : l'ensemble qui contient tous les éléments de $A$ mais aucun élément de $B$ (différence).
- $A \triangle B$ : l'ensemble qui contient tous les éléments de $A$ et $B$ mais aucun élément présent dans les deux ensembles à la fois (différence symétrique).

#### Propriétés sur les Ensembles

Soient $A$ et $B$ deux ensebles :

- $A = B \Leftrightarrow A \subseteq B, B \subseteq A$

#### Remarques

Attention à ne pas confondre appartenance ($\in$) et inclusion ($\subset$) !

## Application

Soient $A$ et $B$ deux ensembles. Une loi (ou application) qui associe chaque élément de $A$ à un élément de $B$ est appelé une application (ou fonction).

**Remarque** : une application peut avoir plusieurs antécédents dans $A$, mais aura une seule image sur $B$.

Soient $A$, $B$ et $C$ trois ensembles et :

- $f : A \rightarrow B$
- $g : B \rightarrow C$.

On définit une application notée $g \circ f$ par : $g \circ f : A \rightarrow C$

### Image réciproque

Soit $f \in \mathscr F (A, B)$. On appelle l'image réciproque de $Y \subset B$ par l'application $f$ l'ensemble :

- $f^{-1}(Y) = \{x \in A | f(x) \in Y\}$
