#!/usr/bin/env bash
# filepath: convergence_muscl_study.sh

# Compiler
echo "Compilation..."
make

if [ ! -f build/projet_c++_S7_Debug ]; then
    echo "Erreur: L'exécutable n'a pas pu être compilé"
    exit 1
fi

echo "===== ÉTUDE DE CONVERGENCE (ESPACE ET TEMPS) - MUSCL ORDRE 2 ====="
echo "Type,Test,deltax,cfl,erreur_h_L2,erreur_q_L2,erreur_h_L1,erreur_q_L1,erreur_h_L2_mask,erreur_q_L2_mask,erreur_h_L1_mask,erreur_q_L1_mask" > convergence_results_muscl.csv

# Créer des fichiers temporaires avec différents CFL - dernier paramètre 2 = MUSCL ordre 2
echo "0. 5. 0.05 premiere_sortie 100 2"  > /tmp/init_test_temps_cfl_05_muscl.txt
echo "0. 5. 0.9 premiere_sortie 100 2"   > /tmp/init_test_temps_cfl_09_muscl.txt
echo "0. 5. 0.45 premiere_sortie 100 2"  > /tmp/init_test_temps_cfl_045_muscl.txt
echo "0. 5. 0.225 premiere_sortie 100 2" > /tmp/init_test_temps_cfl_0225_muscl.txt

# ====== CONVERGENCE SPATIALE (maillage raffiné, même CFL) ======
echo ""
echo "╔═════════════════════════════════════════════╗"
echo "║   TEST 1 MUSCL: CONVERGENCE EN ESPACE       ║"
echo "║   (CFL fixe, maillage raffiné)              ║"
echo "╚═════════════════════════════════════════════╝"

# Test 1a : maillage initial
echo ""
echo "Test 1a MUSCL : Maillage initial (1000 subdivisions, CFL=0.05)"
output1a=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage.txt /tmp/init_test_temps_cfl_05_muscl.txt 2>&1)
deltax1a=$(echo "$output1a" | grep "Delta x:" | tail -1 | awk '{print $NF}')
erreur_h1a=$(echo "$output1a" | grep "Erreur finale sur la hauteur:" | tail -1 | awk '{print $NF}')
erreur_q1a=$(echo "$output1a" | grep "Erreur finale sur le débit:" | tail -1 | awk '{print $NF}')
erreur_h1a_L1=$(echo "$output1a" | grep "Erreur finale l1(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q1a_L1=$(echo "$output1a" | grep "Erreur finale l1(débit):" | tail -1 | awk '{print $NF}')
erreur_h1a_L2_mask=$(echo "$output1a" | grep "Erreur finale l2 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q1a_L2_mask=$(echo "$output1a" | grep "Erreur finale l2 masquée(débit):" | tail -1 | awk '{print $NF}')
erreur_h1a_L1_mask=$(echo "$output1a" | grep "Erreur finale l1 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q1a_L1_mask=$(echo "$output1a" | grep "Erreur finale l1 masquée(débit):" | tail -1 | awk '{print $NF}')
echo "  → Δx=$deltax1a, Erreur h(L2)=$erreur_h1a, Erreur q(L2)=$erreur_q1a, Erreur h(L1)=$erreur_h1a_L1, Erreur q(L1)=$erreur_q1a_L1"
echo "Spatial_MUSCL,1a,$deltax1a,0.05,$erreur_h1a,$erreur_q1a,$erreur_h1a_L1,$erreur_q1a_L1,$erreur_h1a_L2_mask,$erreur_q1a_L2_mask,$erreur_h1a_L1_mask,$erreur_q1a_L1_mask" >> convergence_results_muscl.csv

# Test 1b : maillage raffiné x2
echo ""
echo "Test 1b MUSCL : Maillage x2 (2000 subdivisions, CFL=0.05)"
output1b=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage_2000.txt /tmp/init_test_temps_cfl_05_muscl.txt 2>&1)
deltax1b=$(echo "$output1b" | grep "Delta x:" | tail -1 | awk '{print $NF}')
erreur_h1b=$(echo "$output1b" | grep "Erreur finale sur la hauteur:" | tail -1 | awk '{print $NF}')
erreur_q1b=$(echo "$output1b" | grep "Erreur finale sur le débit:" | tail -1 | awk '{print $NF}')
erreur_h1b_L1=$(echo "$output1b" | grep "Erreur finale l1(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q1b_L1=$(echo "$output1b" | grep "Erreur finale l1(débit):" | tail -1 | awk '{print $NF}')
erreur_h1b_L2_mask=$(echo "$output1b" | grep "Erreur finale l2 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q1b_L2_mask=$(echo "$output1b" | grep "Erreur finale l2 masquée(débit):" | tail -1 | awk '{print $NF}')
erreur_h1b_L1_mask=$(echo "$output1b" | grep "Erreur finale l1 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q1b_L1_mask=$(echo "$output1b" | grep "Erreur finale l1 masquée(débit):" | tail -1 | awk '{print $NF}')
echo "  → Δx=$deltax1b, Erreur h(L2)=$erreur_h1b, Erreur q(L2)=$erreur_q1b, Erreur h(L1)=$erreur_h1b_L1, Erreur q(L1)=$erreur_q1b_L1"
echo "Spatial_MUSCL,1b,$deltax1b,0.05,$erreur_h1b,$erreur_q1b,$erreur_h1b_L1,$erreur_q1b_L1,$erreur_h1b_L2_mask,$erreur_q1b_L2_mask,$erreur_h1b_L1_mask,$erreur_q1b_L1_mask" >> convergence_results_muscl.csv

# ====== CONVERGENCE TEMPORELLE (maillage très fin fixe, CFL divisé par 2) ======
echo ""
echo "╔═════════════════════════════════════════════╗"
echo "║   TEST 2 MUSCL: CONVERGENCE EN TEMPS        ║"
echo "║   (maillage fin N=8000, CFL varié)          ║"
echo "╚═════════════════════════════════════════════╝"

# Test 2a : maillage très fin, CFL = 0.9
echo ""
echo "Test 2a MUSCL : Maillage très fin (8000 subdivisions, CFL=0.9)"
output2a=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage_8000.txt /tmp/init_test_temps_cfl_09_muscl.txt 2>&1)
deltax2a=$(echo "$output2a" | grep "Delta x:" | tail -1 | awk '{print $NF}')
erreur_h2a=$(echo "$output2a" | grep "Erreur finale sur la hauteur:" | tail -1 | awk '{print $NF}')
erreur_q2a=$(echo "$output2a" | grep "Erreur finale sur le débit:" | tail -1 | awk '{print $NF}')
erreur_h2a_L1=$(echo "$output2a" | grep "Erreur finale l1(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q2a_L1=$(echo "$output2a" | grep "Erreur finale l1(débit):" | tail -1 | awk '{print $NF}')
erreur_h2a_L2_mask=$(echo "$output2a" | grep "Erreur finale l2 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q2a_L2_mask=$(echo "$output2a" | grep "Erreur finale l2 masquée(débit):" | tail -1 | awk '{print $NF}')
erreur_h2a_L1_mask=$(echo "$output2a" | grep "Erreur finale l1 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q2a_L1_mask=$(echo "$output2a" | grep "Erreur finale l1 masquée(débit):" | tail -1 | awk '{print $NF}')
echo "  → Δx=$deltax2a, Erreur h(L2)=$erreur_h2a, Erreur q(L2)=$erreur_q2a, Erreur h(L1)=$erreur_h2a_L1, Erreur q(L1)=$erreur_q2a_L1"
echo "Temporal_MUSCL,2a,$deltax2a,0.9,$erreur_h2a,$erreur_q2a,$erreur_h2a_L1,$erreur_q2a_L1,$erreur_h2a_L2_mask,$erreur_q2a_L2_mask,$erreur_h2a_L1_mask,$erreur_q2a_L1_mask" >> convergence_results_muscl.csv

# Test 2b : maillage très fin, CFL = 0.45
echo ""
echo "Test 2b MUSCL : Maillage très fin (8000 subdivisions, CFL=0.45)"
output2b=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage_8000.txt /tmp/init_test_temps_cfl_045_muscl.txt 2>&1)
deltax2b=$(echo "$output2b" | grep "Delta x:" | tail -1 | awk '{print $NF}')
erreur_h2b=$(echo "$output2b" | grep "Erreur finale sur la hauteur:" | tail -1 | awk '{print $NF}')
erreur_q2b=$(echo "$output2b" | grep "Erreur finale sur le débit:" | tail -1 | awk '{print $NF}')
erreur_h2b_L1=$(echo "$output2b" | grep "Erreur finale l1(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q2b_L1=$(echo "$output2b" | grep "Erreur finale l1(débit):" | tail -1 | awk '{print $NF}')
erreur_h2b_L2_mask=$(echo "$output2b" | grep "Erreur finale l2 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q2b_L2_mask=$(echo "$output2b" | grep "Erreur finale l2 masquée(débit):" | tail -1 | awk '{print $NF}')
erreur_h2b_L1_mask=$(echo "$output2b" | grep "Erreur finale l1 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q2b_L1_mask=$(echo "$output2b" | grep "Erreur finale l1 masquée(débit):" | tail -1 | awk '{print $NF}')
echo "  → Δx=$deltax2b, Erreur h(L2)=$erreur_h2b, Erreur q(L2)=$erreur_q2b, Erreur h(L1)=$erreur_h2b_L1, Erreur q(L1)=$erreur_q2b_L1"
echo "Temporal_MUSCL,2b,$deltax2b,0.45,$erreur_h2b,$erreur_q2b,$erreur_h2b_L1,$erreur_q2b_L1,$erreur_h2b_L2_mask,$erreur_q2b_L2_mask,$erreur_h2b_L1_mask,$erreur_q2b_L1_mask" >> convergence_results_muscl.csv

# Test 2c : maillage très fin, CFL = 0.225
echo ""
echo "Test 2c MUSCL : Maillage très fin (8000 subdivisions, CFL=0.225)"
output2c=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage_8000.txt /tmp/init_test_temps_cfl_0225_muscl.txt 2>&1)
deltax2c=$(echo "$output2c" | grep "Delta x:" | tail -1 | awk '{print $NF}')
erreur_h2c=$(echo "$output2c" | grep "Erreur finale sur la hauteur:" | tail -1 | awk '{print $NF}')
erreur_q2c=$(echo "$output2c" | grep "Erreur finale sur le débit:" | tail -1 | awk '{print $NF}')
erreur_h2c_L1=$(echo "$output2c" | grep "Erreur finale l1(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q2c_L1=$(echo "$output2c" | grep "Erreur finale l1(débit):" | tail -1 | awk '{print $NF}')
erreur_h2c_L2_mask=$(echo "$output2c" | grep "Erreur finale l2 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q2c_L2_mask=$(echo "$output2c" | grep "Erreur finale l2 masquée(débit):" | tail -1 | awk '{print $NF}')
erreur_h2c_L1_mask=$(echo "$output2c" | grep "Erreur finale l1 masquée(hauteur):" | tail -1 | awk '{print $NF}')
erreur_q2c_L1_mask=$(echo "$output2c" | grep "Erreur finale l1 masquée(débit):" | tail -1 | awk '{print $NF}')
echo "  → Δx=$deltax2c, Erreur h(L2)=$erreur_h2c, Erreur q(L2)=$erreur_q2c, Erreur h(L1)=$erreur_h2c_L1, Erreur q(L1)=$erreur_q2c_L1"
echo "Temporal_MUSCL,2c,$deltax2c,0.225,$erreur_h2c,$erreur_q2c,$erreur_h2c_L1,$erreur_q2c_L1,$erreur_h2c_L2_mask,$erreur_q2c_L2_mask,$erreur_h2c_L1_mask,$erreur_q2c_L1_mask" >> convergence_results_muscl.csv

# ====== CALCULS DES ORDRES DE CONVERGENCE ======
echo ""
echo "╔═════════════════════════════════════════════╗"
echo "║   RÉSULTATS CONVERGENCE MUSCL               ║"
echo "╚═════════════════════════════════════════════╝"

# Fonctions robustes (gèrent les notations scientifiques e+xx)
safe_ratio() {
    awk -v a="$1" -v b="$2" 'BEGIN {
        if (a=="" || b=="" || !(a+0==a) || !(b+0==b) || b==0) { print "nan"; }
        else { printf "%.12g", a/b; }
    }'
}

safe_order() {
    awk -v re="$1" -v rx="$2" 'BEGIN {
        if (re=="" || rx=="" || !(re+0==re) || !(rx+0==rx) || re<=0 || rx<=0 || rx==1) { print "nan"; }
        else { printf "%.6f", log(re)/log(rx); }
    }'
}

# Convergence spatiale
echo ""
echo "═══ CONVERGENCE EN ESPACE (MUSCL) ═══"
ratio_deltax=$(safe_ratio "$deltax1a" "$deltax1b")
ratio_erreur_h_spatial=$(safe_ratio "$erreur_h1a" "$erreur_h1b")
ratio_erreur_q_spatial=$(safe_ratio "$erreur_q1a" "$erreur_q1b")
ratio_erreur_h_spatial_L1=$(safe_ratio "$erreur_h1a_L1" "$erreur_h1b_L1")
ratio_erreur_q_spatial_L1=$(safe_ratio "$erreur_q1a_L1" "$erreur_q1b_L1")

ordre_h_spatial=$(safe_order "$ratio_erreur_h_spatial" "$ratio_deltax")
ordre_q_spatial=$(safe_order "$ratio_erreur_q_spatial" "$ratio_deltax")
ordre_h_spatial_L1=$(safe_order "$ratio_erreur_h_spatial_L1" "$ratio_deltax")
ordre_q_spatial_L1=$(safe_order "$ratio_erreur_q_spatial_L1" "$ratio_deltax")

echo "Rapport Δx: $ratio_deltax (doit être ≈ 2)"
echo "Rapport erreur hauteur (L2): $ratio_erreur_h_spatial"
echo "Rapport erreur débit   (L2): $ratio_erreur_q_spatial"
echo "Rapport erreur hauteur (L1): $ratio_erreur_h_spatial_L1"
echo "Rapport erreur débit   (L1): $ratio_erreur_q_spatial_L1"
echo ""
printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre spatial MUSCL" "Valeur (≈2.0)"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18s │\n" "  Hauteur (L2)" "$ordre_h_spatial"
printf "│ %-20s │ %18s │\n" "  Débit   (L2)" "$ordre_q_spatial"
printf "│ %-20s │ %18s │\n" "  Hauteur (L1)" "$ordre_h_spatial_L1"
printf "│ %-20s │ %18s │\n" "  Débit   (L1)" "$ordre_q_spatial_L1"
printf "└─────────────────────────────────────────────┘\n"

# Convergence temporelle
echo ""
echo "═══ CONVERGENCE EN TEMPS (MUSCL) ═══"
ratio_erreur_h_temporal=$(safe_ratio "$erreur_h2a" "$erreur_h2b")
ratio_erreur_q_temporal=$(safe_ratio "$erreur_q2a" "$erreur_q2b")
ratio_erreur_h_temporal_L1=$(safe_ratio "$erreur_h2a_L1" "$erreur_h2b_L1")
ratio_erreur_q_temporal_L1=$(safe_ratio "$erreur_q2a_L1" "$erreur_q2b_L1")
ratio_cfl=$(safe_ratio "0.9" "0.45")

# Masqué (excluant discontinuités)
ratio_erreur_h_temporal_mask_L2=$(safe_ratio "$erreur_h2a_L2_mask" "$erreur_h2b_L2_mask")
ratio_erreur_q_temporal_mask_L2=$(safe_ratio "$erreur_q2a_L2_mask" "$erreur_q2b_L2_mask")
ratio_erreur_h_temporal_mask_L1=$(safe_ratio "$erreur_h2a_L1_mask" "$erreur_h2b_L1_mask")
ratio_erreur_q_temporal_mask_L1=$(safe_ratio "$erreur_q2a_L1_mask" "$erreur_q2b_L1_mask")

ordre_h_temporal=$(safe_order "$ratio_erreur_h_temporal" "$ratio_cfl")
ordre_q_temporal=$(safe_order "$ratio_erreur_q_temporal" "$ratio_cfl")
ordre_h_temporal_L1=$(safe_order "$ratio_erreur_h_temporal_L1" "$ratio_cfl")
ordre_q_temporal_L1=$(safe_order "$ratio_erreur_q_temporal_L1" "$ratio_cfl")

ordre_h_temporal_mask_L2=$(safe_order "$ratio_erreur_h_temporal_mask_L2" "$ratio_cfl")
ordre_q_temporal_mask_L2=$(safe_order "$ratio_erreur_q_temporal_mask_L2" "$ratio_cfl")
ordre_h_temporal_mask_L1=$(safe_order "$ratio_erreur_h_temporal_mask_L1" "$ratio_cfl")
ordre_q_temporal_mask_L1=$(safe_order "$ratio_erreur_q_temporal_mask_L1" "$ratio_cfl")

# Calcul ordre entre 2b et 2c aussi
ratio_erreur_h_temporal_2=$(safe_ratio "$erreur_h2b" "$erreur_h2c")
ratio_erreur_q_temporal_2=$(safe_ratio "$erreur_q2b" "$erreur_q2c")
ratio_erreur_h_temporal_2_L1=$(safe_ratio "$erreur_h2b_L1" "$erreur_h2c_L1")
ratio_erreur_q_temporal_2_L1=$(safe_ratio "$erreur_q2b_L1" "$erreur_q2c_L1")
ratio_cfl_2=$(safe_ratio "0.45" "0.225")

# Masqué (2b→2c)
ratio_erreur_h_temporal_2_mask_L2=$(safe_ratio "$erreur_h2b_L2_mask" "$erreur_h2c_L2_mask")
ratio_erreur_q_temporal_2_mask_L2=$(safe_ratio "$erreur_q2b_L2_mask" "$erreur_q2c_L2_mask")
ratio_erreur_h_temporal_2_mask_L1=$(safe_ratio "$erreur_h2b_L1_mask" "$erreur_h2c_L1_mask")
ratio_erreur_q_temporal_2_mask_L1=$(safe_ratio "$erreur_q2b_L1_mask" "$erreur_q2c_L1_mask")

ordre_h_temporal_2=$(safe_order "$ratio_erreur_h_temporal_2" "$ratio_cfl_2")
ordre_q_temporal_2=$(safe_order "$ratio_erreur_q_temporal_2" "$ratio_cfl_2")
ordre_h_temporal_2_L1=$(safe_order "$ratio_erreur_h_temporal_2_L1" "$ratio_cfl_2")
ordre_q_temporal_2_L1=$(safe_order "$ratio_erreur_q_temporal_2_L1" "$ratio_cfl_2")

ordre_h_temporal_2_mask_L2=$(safe_order "$ratio_erreur_h_temporal_2_mask_L2" "$ratio_cfl_2")
ordre_q_temporal_2_mask_L2=$(safe_order "$ratio_erreur_q_temporal_2_mask_L2" "$ratio_cfl_2")
ordre_h_temporal_2_mask_L1=$(safe_order "$ratio_erreur_h_temporal_2_mask_L1" "$ratio_cfl_2")
ordre_q_temporal_2_mask_L1=$(safe_order "$ratio_erreur_q_temporal_2_mask_L1" "$ratio_cfl_2")

echo "Rapport CFL (0.9→0.45): $ratio_cfl (doit être ≈ 2)"
echo "Rapport erreur hauteur (L2): $ratio_erreur_h_temporal"
echo "Rapport erreur débit   (L2): $ratio_erreur_q_temporal"
echo "Rapport erreur hauteur (L1): $ratio_erreur_h_temporal_L1"
echo "Rapport erreur débit   (L1): $ratio_erreur_q_temporal_L1"
echo ""
printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre temporel MUSCL" "0.9→0.45 (≈1.0)"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18s │\n" "  Hauteur (L2)" "$ordre_h_temporal"
printf "│ %-20s │ %18s │\n" "  Débit   (L2)" "$ordre_q_temporal"
printf "│ %-20s │ %18s │\n" "  Hauteur (L1)" "$ordre_h_temporal_L1"
printf "│ %-20s │ %18s │\n" "  Débit   (L1)" "$ordre_q_temporal_L1"
printf "└─────────────────────────────────────────────┘\n"

printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre temporel MUSCL" "Masqué 0.9→0.45"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18s │\n" "  Hauteur (L2)" "$ordre_h_temporal_mask_L2"
printf "│ %-20s │ %18s │\n" "  Débit   (L2)" "$ordre_q_temporal_mask_L2"
printf "│ %-20s │ %18s │\n" "  Hauteur (L1)" "$ordre_h_temporal_mask_L1"
printf "│ %-20s │ %18s │\n" "  Débit   (L1)" "$ordre_q_temporal_mask_L1"
printf "└─────────────────────────────────────────────┘\n"

echo ""
echo "Rapport CFL (0.45→0.225): $ratio_cfl_2 (doit être ≈ 2)"
echo "Rapport erreur hauteur (L2): $ratio_erreur_h_temporal_2"
echo "Rapport erreur débit   (L2): $ratio_erreur_q_temporal_2"
echo "Rapport erreur hauteur (L1): $ratio_erreur_h_temporal_2_L1"
echo "Rapport erreur débit   (L1): $ratio_erreur_q_temporal_2_L1"
echo ""
printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre temporel MUSCL" "0.45→0.225 (≈1.0)"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18s │\n" "  Hauteur (L2)" "$ordre_h_temporal_2"
printf "│ %-20s │ %18s │\n" "  Débit   (L2)" "$ordre_q_temporal_2"
printf "│ %-20s │ %18s │\n" "  Hauteur (L1)" "$ordre_h_temporal_2_L1"
printf "│ %-20s │ %18s │\n" "  Débit   (L1)" "$ordre_q_temporal_2_L1"
printf "└─────────────────────────────────────────────┘\n"

printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre temporel MUSCL" "Masqué 0.45→0.225"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18s │\n" "  Hauteur (L2)" "$ordre_h_temporal_2_mask_L2"
printf "│ %-20s │ %18s │\n" "  Débit   (L2)" "$ordre_q_temporal_2_mask_L2"
printf "│ %-20s │ %18s │\n" "  Hauteur (L1)" "$ordre_h_temporal_2_mask_L1"
printf "│ %-20s │ %18s │\n" "  Débit   (L1)" "$ordre_q_temporal_2_mask_L1"
printf "└─────────────────────────────────────────────┘\n"

echo ""
echo "✓ Résultats MUSCL sauvegardés dans convergence_results_muscl.csv"
