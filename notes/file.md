# Complexité

Complexité des algorithmes

## Somme des nombres de `1` à `n`

Cherchons l'algorithme avec le moins d'étapes élémentaires

### Utiliser une boucle

```py
i = 1
v = 0
for i in range(i, n):
  v += v + 1

print(v)
```

Coût: `n * 2` opérations

### Utiliser les maths

```py
v = n + 1
v *= b
v /= 2
```

Coût: `3` opérations
