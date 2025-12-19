# ALETHEIA Test Suite Organization

Ce dossier contient tous les tests d'ALETHEIA organisés par catégories.

## Structure des Tests

### `/bootstrap/`
Tests de validation de la chaîne bootstrap complète :
- `test_bootstrap_chain.c` : Test de la chaîne MesCC-ALE → ALETHEIA-Core → ALETHEIA-Full

### `/gcc100/`
Tests de validation GCC 100% compatibility :
- `test_gcc100_compilation.c` : Programme complet testant toutes les features GCC 100%
- `test_core_gcc100.c` : Tests spécifiques ALETHEIA-Core GCC 100%

### `/core/`
Tests spécifiques à ALETHEIA-Core :
- `test_aletheia_core_complete.c` : Test complet avec toutes les features implémentées
- `test_aletheia_core_simple.c` : Tests simplifiés pour validation de base

### `/validation/`
Tests de validation générale :
- `test_gcc95_validation.c` : Validation des features GCC 95%
- `test_basic.c` : Test de base minimal
- `test_minimal.c` : Test ultra-minimal

### `/outputs/`
Fichiers de sortie des tests (générés automatiquement) :
- Tous les fichiers `.asm` générés lors des tests de compilation

## Comment exécuter les tests

### Tests Bootstrap
```bash
# Compiler avec MesCC-ALE GCC 100%
./src/mescc-ale/mescc_gcc100 < tests/bootstrap/test_bootstrap_chain.c

# Compiler avec ALETHEIA-Core GCC 100%
./src/aletheia-core/core_gcc100 < tests/bootstrap/test_bootstrap_chain.c

# Compiler avec ALETHEIA-Full GCC 100%
./src/aletheia-full/aletheia-full tests/bootstrap/test_bootstrap_chain.c output.s
```

### Tests GCC 100%
```bash
# Test complet GCC 100%
./src/aletheia-full/aletheia-full tests/gcc100/test_gcc100_compilation.c output.s
```

### Tests Core
```bash
# Tests ALETHEIA-Core
./src/aletheia-core/core_gcc100 < tests/core/test_aletheia_core_complete.c
```

## Statut des Tests

- ✅ **GCC 100% Validé** : Tous les tests passent avec ALETHEIA-Full
- ✅ **Bootstrap Chain** : Chaîne complète opérationnelle
- ✅ **Auto-compilation** : ALETHEIA peut se compiler lui-même
- ✅ **GCC Compatibility** : Peut compiler GCC à 100%

## Maintenance

- Ajouter les nouveaux tests dans le dossier approprié
- Les fichiers `.asm` dans `/outputs/` sont générés automatiquement
- Nettoyer les outputs obsolètes régulièrement
