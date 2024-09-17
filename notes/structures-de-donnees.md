> Ce fichier est sous license [Creative Commons Attribution 4.0 (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/deed.fr).

# Structures de données et algorithmes

Les langages de programmation modernes implémentent des abstractions haut-niveau dont le coût réel est masqué. L'objet de ce cours est d'évaluer le nombre d'opérations nécessaires pour un algorithme donné à un bas-niveau.

## Complexité

Voici un algorithme:

```py
i = 1
while i <= n:
  i += 2
```

Combien d'affectations fait cet algorithme ?

- `n = 0`: 1 affectation
- `n = 1`: 2 affectations
- `n = 2`: 2 affectations
- `n = 3`: 3 affectations
- `n = 4`: 3 affectations
- `n = 5`: 4 affectations
- `n = 6`: 4 affectations

On peut donc prédire de manière **quasi-certaine** que le nombre d'affectations correspond à `ceil(n) + 1`.

