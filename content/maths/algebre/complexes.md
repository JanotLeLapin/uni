---
title: Complexes
katex: true
---

# Nombres Complexes

Une extension de l'ensemble des nombres réels, avec un nombre particulier imaginaire $i$.

## Définition

Il existe un ensemble que l'on notera $\mathbb{C}$ dont les éléments sont appelés **nombres complexes**.

1. $\mathbb{R} \subset \mathbb{C}$.
2. $\mathbb{C}$ possède un élément $i$ qui vérifie l'équation $i^2 = -1$.
3. Dans $\mathbb{C}$ on peut définir deux opérateurs $+$ et $\times$ généralant celles de $\mathbb{R}$ si on prend en compte le fait que $i^2 = -1$.
4. Tout nombre complexe $z \in \mathbb{C}$ s'écrit de façon unique $z = a + ib$ avec $a$ et $b$ sont des réels.

### Opérations sur les Complexes

Pour tout $z, z', z'' \in \mathbb{C}$, on a :

1. $(z + z') + z'' = z + (z' + z'')$ (associativité)
2. $z + 0 = 0 + z = z$ (existence d'un élément neutre)
3. $z + (-z') = (-z') = z = 0$ (existence d'une symétrie)
4. $z + z' = z' + z$ (commutativité)
5. $(zz')z'' = z(z'z'')$ (associativité)
6. $z \times 1 = 1 \times z = z$ (élément neutre)
7. $z \times (z' + z'') = zz' + zz''$ (distributivité)
8. $zz' = z'z$ (commutivité)

## Réel et Imaginaire

- Le réel $a$ s'appelle la **partie réelle** de z. On le note $Re(z)$.
- Le réel $b$ s'appelle la **partie imaginaire** de z. On le note $Im(z)$.
- L'écriture $z = a + ib$ s'appelle l'écriture algébrique.

### Proposition

Pour tout $z, z' \in \mathbb{C}$ et tout $\lambda \in \mathbb{R}$ :

- $Re(z + \lambda z') = Re(z) + \lambda Re(z')$
- $Im(z + \lambda z') = Im(z) + \lambda Im(z')$

## Conjuguaison

Soit $z = a + ib \in \mathbb{C}$ avec $a, b \in \mathbb{R}$.

On appelle le nombre complexe **conjugué** de $z$ le nombre $(a - ib)$ que l'on note $\overline{z}$.

### Exemples

- $\overline{3 - 5i} = 3 + 5i$
- $\overline{(3i - 4)} = -3i -4$

### Propositions

Pour tous $z, z' \in \mathbb{C}$ on a :

1. $\overline{\overline{z}} = z$
2. $\overline{z + z'} = \overline{z} + \overline{z'}$
3. $\overline{z \times z'} = \overline{z} \times \overline{z'}$
4. $\frac{\overline{z}}{\overline{z'}} = \frac{z}{z'}$

## Affixe d'un point ou vecteur

Soit $z \in \mathbb{C}$ avec $z = x + iy$ où $x, y \in \mathbb{R}$

- Le point $M(x; y)$ est appelé le **point image** de $z$ et se note $M(z)$ ou $M_z$.
- Le vecteur $\overrightarrow{v(x, y)}$ est appelé le **vecteur image** de $z$ et se note $\overrightarrow{v(z)}$ ou $\overrightarrow{v_z}$.

On a donc $\overrightarrow{v_z} = \overrightarrow{OM_z}$

### Proposition

1. Pour tout $z, z' \in \mathbb{C}$ et pour tout $\lambda \in \mathbb{R}$ : $\overrightarrow{v_{z + \lambda z'}} = \overrightarrow{v_z} + \lambda \overrightarrow{v_z'}$.
2. Si $A$ et $B$ sont des points d'affixes respectifs $z_A$ et $z_B$ alors le vecteur $\overrightarrow{AB}$ a pour affixes $z_b - z_a$.
