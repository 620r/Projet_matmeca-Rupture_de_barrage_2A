# filepath: convergence_study.sh

# Compiler
echo "Compilation..."
make

if [ ! -f build/projet_c++_S7_Debug ]; then
    echo "Erreur: L'exécutable n'a pas pu être compilé"
    exit 1
fi

echo "===== ÉTUDE DE CONVERGENCE (ESPACE ET TEMPS) ====="
echo "Type,Test,deltax,cfl,erreur_h_L2,erreur_q_L2,erreur_h_L1,erreur_q_L1,erreur_h_L2_mask,erreur_q_L2_mask,erreur_h_L1_mask,erreur_q_L1_mask" > convergence_results.csv

# Créer des fichiers temporaires avec différents CFL
echo "0. 5. 0.05 premiere_sortie 100" > /tmp/init_test_temps_cfl_05.txt
echo "0. 5. 0.9 premiere_sortie 100" > /tmp/init_test_temps_cfl_09.txt
echo "0. 5. 0.45 premiere_sortie 100" > /tmp/init_test_temps_cfl_045.txt
echo "0. 5. 0.225 premiere_sortie 100" > /tmp/init_test_temps_cfl_0225.txt

# ====== CONVERGENCE SPATIALE (maillage raffiné, même CFL) ======
echo ""
echo "╔═════════════════════════════════════╗"
echo "║   TEST 1: CONVERGENCE EN ESPACE     ║"
echo "║   (CFL fixe, maillage raffiné)      ║"
echo "╚═════════════════════════════════════╝"

# Test 1a : maillage initial
echo ""
echo "Test 1a : Maillage initial (1000 subdivisions, CFL=0.05)"
output1a=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage.txt /tmp/init_test_temps_cfl_05.txt 2>&1)
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
echo "Spatial,1a,$deltax1a,0.05,$erreur_h1a,$erreur_q1a,$erreur_h1a_L1,$erreur_q1a_L1,$erreur_h1a_L2_mask,$erreur_q1a_L2_mask,$erreur_h1a_L1_mask,$erreur_q1a_L1_mask" >> convergence_results.csv

# Test 1b : maillage raffiné x2
echo ""
echo "Test 1b : Maillage x2 (2000 subdivisions, CFL=0.05)"
output1b=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage_2000.txt /tmp/init_test_temps_cfl_05.txt 2>&1)
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
echo "Spatial,1b,$deltax1b,0.05,$erreur_h1b,$erreur_q1b,$erreur_h1b_L1,$erreur_q1b_L1,$erreur_h1b_L2_mask,$erreur_q1b_L2_mask,$erreur_h1b_L1_mask,$erreur_q1b_L1_mask" >> convergence_results.csv

# ====== CONVERGENCE TEMPORELLE (maillage très fin fixe, CFL divisé par 2) ======
echo ""
echo "╔═════════════════════════════════════╗"
echo "║   TEST 2: CONVERGENCE EN TEMPS      ║"
echo "║   (maillage fin N=8000, CFL varié)  ║"
echo "╚═════════════════════════════════════╝"

# Test 2a : maillage très fin, CFL = 0.9
echo ""
echo "Test 2a : Maillage très fin (8000 subdivisions, CFL=0.9)"
output2a=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage_8000.txt /tmp/init_test_temps_cfl_09.txt 2>&1)
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
echo "Temporal,2a,$deltax2a,0.9,$erreur_h2a,$erreur_q2a,$erreur_h2a_L1,$erreur_q2a_L1,$erreur_h2a_L2_mask,$erreur_q2a_L2_mask,$erreur_h2a_L1_mask,$erreur_q2a_L1_mask" >> convergence_results.csv

# Test 2b : maillage très fin, CFL = 0.45
echo ""
echo "Test 2b : Maillage très fin (8000 subdivisions, CFL=0.45)"
output2b=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage_8000.txt /tmp/init_test_temps_cfl_045.txt 2>&1)
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
echo "Temporal,2b,$deltax2b,0.45,$erreur_h2b,$erreur_q2b,$erreur_h2b_L1,$erreur_q2b_L1,$erreur_h2b_L2_mask,$erreur_q2b_L2_mask,$erreur_h2b_L1_mask,$erreur_q2b_L1_mask" >> convergence_results.csv

# Test 2c : maillage très fin, CFL = 0.225
echo ""
echo "Test 2c : Maillage très fin (8000 subdivisions, CFL=0.225)"
output2c=$(./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage_8000.txt /tmp/init_test_temps_cfl_0225.txt 2>&1)
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
echo "Temporal,2c,$deltax2c,0.225,$erreur_h2c,$erreur_q2c,$erreur_h2c_L1,$erreur_q2c_L1,$erreur_h2c_L2_mask,$erreur_q2c_L2_mask,$erreur_h2c_L1_mask,$erreur_q2c_L1_mask" >> convergence_results.csv

# ====== CALCULS DES ORDRES DE CONVERGENCE ======
echo ""
echo "╔═════════════════════════════════════╗"
echo "║   RÉSULTATS CONVERGENCE             ║"
echo "╚═════════════════════════════════════╝"

# Convergence spatiale
echo ""
echo "═══ CONVERGENCE EN ESPACE ═══"
ratio_deltax=$(echo "scale=6; $deltax1a / $deltax1b" | bc)
ratio_erreur_h_spatial=$(echo "scale=6; $erreur_h1a / $erreur_h1b" | bc)
ratio_erreur_q_spatial=$(echo "scale=6; $erreur_q1a / $erreur_q1b" | bc)
ratio_erreur_h_spatial_L1=$(echo "scale=6; $erreur_h1a_L1 / $erreur_h1b_L1" | bc)
ratio_erreur_q_spatial_L1=$(echo "scale=6; $erreur_q1a_L1 / $erreur_q1b_L1" | bc)

ordre_h_spatial=$(echo "scale=4; l($ratio_erreur_h_spatial) / l($ratio_deltax)" | bc -l)
ordre_q_spatial=$(echo "scale=4; l($ratio_erreur_q_spatial) / l($ratio_deltax)" | bc -l)
ordre_h_spatial_L1=$(echo "scale=4; l($ratio_erreur_h_spatial_L1) / l($ratio_deltax)" | bc -l)
ordre_q_spatial_L1=$(echo "scale=4; l($ratio_erreur_q_spatial_L1) / l($ratio_deltax)" | bc -l)

echo "Rapport Δx: $ratio_deltax (doit être ≈ 2)"
echo "Rapport erreur hauteur (L2): $ratio_erreur_h_spatial"
echo "Rapport erreur débit   (L2): $ratio_erreur_q_spatial"
echo "Rapport erreur hauteur (L1): $ratio_erreur_h_spatial_L1"
echo "Rapport erreur débit   (L1): $ratio_erreur_q_spatial_L1"
echo ""
printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre spatial" "Valeur (≈1.0)"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L2)" "$ordre_h_spatial"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L2)" "$ordre_q_spatial"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L1)" "$ordre_h_spatial_L1"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L1)" "$ordre_q_spatial_L1"
printf "└─────────────────────────────────────────────┘\n"

# Convergence temporelle
echo ""
echo "═══ CONVERGENCE EN TEMPS ═══"
ratio_erreur_h_temporal=$(echo "scale=6; $erreur_h2a / $erreur_h2b" | bc)
ratio_erreur_q_temporal=$(echo "scale=6; $erreur_q2a / $erreur_q2b" | bc)
ratio_erreur_h_temporal_L1=$(echo "scale=6; $erreur_h2a_L1 / $erreur_h2b_L1" | bc)
ratio_erreur_q_temporal_L1=$(echo "scale=6; $erreur_q2a_L1 / $erreur_q2b_L1" | bc)
ratio_cfl=$(echo "scale=6; 0.9 / 0.45" | bc)

# Masqué (excluant discontinuités)
ratio_erreur_h_temporal_mask_L2=$(echo "scale=6; $erreur_h2a_L2_mask / $erreur_h2b_L2_mask" | bc)
ratio_erreur_q_temporal_mask_L2=$(echo "scale=6; $erreur_q2a_L2_mask / $erreur_q2b_L2_mask" | bc)
ratio_erreur_h_temporal_mask_L1=$(echo "scale=6; $erreur_h2a_L1_mask / $erreur_h2b_L1_mask" | bc)
ratio_erreur_q_temporal_mask_L1=$(echo "scale=6; $erreur_q2a_L1_mask / $erreur_q2b_L1_mask" | bc)

ordre_h_temporal=$(echo "scale=4; l($ratio_erreur_h_temporal) / l($ratio_cfl)" | bc -l)
ordre_q_temporal=$(echo "scale=4; l($ratio_erreur_q_temporal) / l($ratio_cfl)" | bc -l)
ordre_h_temporal_L1=$(echo "scale=4; l($ratio_erreur_h_temporal_L1) / l($ratio_cfl)" | bc -l)
ordre_q_temporal_L1=$(echo "scale=4; l($ratio_erreur_q_temporal_L1) / l($ratio_cfl)" | bc -l)

ordre_h_temporal_mask_L2=$(echo "scale=4; l($ratio_erreur_h_temporal_mask_L2) / l($ratio_cfl)" | bc -l)
ordre_q_temporal_mask_L2=$(echo "scale=4; l($ratio_erreur_q_temporal_mask_L2) / l($ratio_cfl)" | bc -l)
ordre_h_temporal_mask_L1=$(echo "scale=4; l($ratio_erreur_h_temporal_mask_L1) / l($ratio_cfl)" | bc -l)
ordre_q_temporal_mask_L1=$(echo "scale=4; l($ratio_erreur_q_temporal_mask_L1) / l($ratio_cfl)" | bc -l)

# Calcul ordre entre 2b et 2c aussi
ratio_erreur_h_temporal_2=$(echo "scale=6; $erreur_h2b / $erreur_h2c" | bc)
ratio_erreur_q_temporal_2=$(echo "scale=6; $erreur_q2b / $erreur_q2c" | bc)
ratio_erreur_h_temporal_2_L1=$(echo "scale=6; $erreur_h2b_L1 / $erreur_h2c_L1" | bc)
ratio_erreur_q_temporal_2_L1=$(echo "scale=6; $erreur_q2b_L1 / $erreur_q2c_L1" | bc)
ratio_cfl_2=$(echo "scale=6; 0.45 / 0.225" | bc)

# Masqué (2b→2c)
ratio_erreur_h_temporal_2_mask_L2=$(echo "scale=6; $erreur_h2b_L2_mask / $erreur_h2c_L2_mask" | bc)
ratio_erreur_q_temporal_2_mask_L2=$(echo "scale=6; $erreur_q2b_L2_mask / $erreur_q2c_L2_mask" | bc)
ratio_erreur_h_temporal_2_mask_L1=$(echo "scale=6; $erreur_h2b_L1_mask / $erreur_h2c_L1_mask" | bc)
ratio_erreur_q_temporal_2_mask_L1=$(echo "scale=6; $erreur_q2b_L1_mask / $erreur_q2c_L1_mask" | bc)

ordre_h_temporal_2=$(echo "scale=4; l($ratio_erreur_h_temporal_2) / l($ratio_cfl_2)" | bc -l)
ordre_q_temporal_2=$(echo "scale=4; l($ratio_erreur_q_temporal_2) / l($ratio_cfl_2)" | bc -l)
ordre_h_temporal_2_L1=$(echo "scale=4; l($ratio_erreur_h_temporal_2_L1) / l($ratio_cfl_2)" | bc -l)
ordre_q_temporal_2_L1=$(echo "scale=4; l($ratio_erreur_q_temporal_2_L1) / l($ratio_cfl_2)" | bc -l)

ordre_h_temporal_2_mask_L2=$(echo "scale=4; l($ratio_erreur_h_temporal_2_mask_L2) / l($ratio_cfl_2)" | bc -l)
ordre_q_temporal_2_mask_L2=$(echo "scale=4; l($ratio_erreur_q_temporal_2_mask_L2) / l($ratio_cfl_2)" | bc -l)
ordre_h_temporal_2_mask_L1=$(echo "scale=4; l($ratio_erreur_h_temporal_2_mask_L1) / l($ratio_cfl_2)" | bc -l)
ordre_q_temporal_2_mask_L1=$(echo "scale=4; l($ratio_erreur_q_temporal_2_mask_L1) / l($ratio_cfl_2)" | bc -l)

echo "Rapport CFL (0.9→0.45): $ratio_cfl (doit être ≈ 2)"
echo "Rapport erreur hauteur (L2): $ratio_erreur_h_temporal"
echo "Rapport erreur débit   (L2): $ratio_erreur_q_temporal"
echo "Rapport erreur hauteur (L1): $ratio_erreur_h_temporal_L1"
echo "Rapport erreur débit   (L1): $ratio_erreur_q_temporal_L1"
echo ""
printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre temporel" "0.9→0.45 (≈1.0)"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L2)" "$ordre_h_temporal"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L2)" "$ordre_q_temporal"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L1)" "$ordre_h_temporal_L1"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L1)" "$ordre_q_temporal_L1"
printf "└─────────────────────────────────────────────┘\n"

printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre temporel" "Masqué 0.9→0.45"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L2)" "$ordre_h_temporal_mask_L2"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L2)" "$ordre_q_temporal_mask_L2"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L1)" "$ordre_h_temporal_mask_L1"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L1)" "$ordre_q_temporal_mask_L1"
printf "└─────────────────────────────────────────────┘\n"

echo ""
echo "Rapport CFL (0.45→0.225): $ratio_cfl_2 (doit être ≈ 2)"
echo "Rapport erreur hauteur (L2): $ratio_erreur_h_temporal_2"
echo "Rapport erreur débit   (L2): $ratio_erreur_q_temporal_2"
echo "Rapport erreur hauteur (L1): $ratio_erreur_h_temporal_2_L1"
echo "Rapport erreur débit   (L1): $ratio_erreur_q_temporal_2_L1"
echo ""
printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre temporel" "0.45→0.225 (≈1.0)"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L2)" "$ordre_h_temporal_2"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L2)" "$ordre_q_temporal_2"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L1)" "$ordre_h_temporal_2_L1"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L1)" "$ordre_q_temporal_2_L1"
printf "└─────────────────────────────────────────────┘\n"

printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-20s │ %-18s │\n" "  Ordre temporel" "Masqué 0.45→0.225"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L2)" "$ordre_h_temporal_2_mask_L2"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L2)" "$ordre_q_temporal_2_mask_L2"
printf "│ %-20s │ %18.4f │\n" "  Hauteur (L1)" "$ordre_h_temporal_2_mask_L1"
printf "│ %-20s │ %18.4f │\n" "  Débit   (L1)" "$ordre_q_temporal_2_mask_L1"
printf "└─────────────────────────────────────────────┘\n"

echo ""
echo "✓ Résultats détaillés sauvegardés dans convergence_results.csv"