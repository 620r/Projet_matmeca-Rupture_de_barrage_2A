#!/usr/bin/env bash
# filepath: convergence_muscl_study.sh

# Study objective:
# - Spatial convergence by mesh refinement campaign
# - Temporal convergence by CFL refinement campaign (CFL <= 0.45)
# - Order estimated with log-log linear regression for robustness

echo "Compilation..."
make

if [ ! -f build/projet_c++_S7_Debug ]; then
    echo "Erreur: L'executable n'a pas pu etre compile"
    exit 1
fi

echo "===== ETUDE DE CONVERGENCE (ESPACE ET TEMPS) - MUSCL ORDRE 2 ====="
echo "Type,Test,deltax,cfl,erreur_h_L2,erreur_q_L2,erreur_h_L1,erreur_q_L1,erreur_h_L2_mask,erreur_q_L2_mask,erreur_h_L1_mask,erreur_q_L1_mask" > convergence_results_muscl.csv

TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

create_mesh_init() {
    local n="$1"
    local file="$2"
    cat > "$file" <<EOF
20
50

$n

10
1
EOF
}

create_time_init() {
    local cfl="$1"
    local file="$2"
    # Last parameter 2 = MUSCL order 2
    echo "0. 5. $cfl premiere_sortie 100 2" > "$file"
}

extract_metric() {
    local text="$1"
    local label="$2"
    local value

    value=$(printf "%s\n" "$text" | grep -F "$label" | tail -1 | awk '{print $NF}')
    if [ -z "$value" ]; then
        echo "nan"
    else
        echo "$value"
    fi
}

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

regression_order() {
    local x_values="$1"
    local y_values="$2"

    awk -v xs="$x_values" -v ys="$y_values" 'BEGIN {
        nx = split(xs, xarr, " ");
        ny = split(ys, yarr, " ");
        if (nx != ny || nx < 2) { print "nan"; exit; }

        n = 0;
        sumx = 0; sumy = 0; sumxx = 0; sumxy = 0;

        for (i=1; i<=nx; i++) {
            x = xarr[i] + 0;
            y = yarr[i] + 0;
            if (!(x > 0) || !(y > 0)) { continue; }

            lx = log(x);
            ly = log(y);

            n++;
            sumx += lx;
            sumy += ly;
            sumxx += lx * lx;
            sumxy += lx * ly;
        }

        if (n < 2) { print "nan"; exit; }

        den = n * sumxx - sumx * sumx;
        if (den == 0) { print "nan"; exit; }

        slope = (n * sumxy - sumx * sumy) / den;
        printf "%.6f", slope;
    }'
}

run_solver() {
    local mesh_init="$1"
    local time_init="$2"

    local output
    output=$(./build/projet_c++_S7_Debug "$mesh_init" "$time_init" 2>&1)
    local status=$?

    if [ "$status" -ne 0 ]; then
        echo "ERREUR_SOLVEUR"
        echo "$output"
        return "$status"
    fi

    echo "$output"
    return 0
}

# -----------------------------
# CAMPAGNE SPATIALE (CFL fixe)
# -----------------------------
echo ""
echo "╔═════════════════════════════════════════════╗"
echo "║   CAMPAGNE MUSCL: CONVERGENCE EN ESPACE     ║"
echo "║   (CFL fixe, maillage raffine)              ║"
echo "╚═════════════════════════════════════════════╝"

SPATIAL_CFL="0.05"
SPATIAL_N_LIST="1000 2000 4000"
SPATIAL_TIME_INIT="$TMP_DIR/time_spatial.txt"
create_time_init "$SPATIAL_CFL" "$SPATIAL_TIME_INIT"

spatial_dxs=""
spatial_ehL2=""
spatial_eqL2=""
spatial_ehL1=""
spatial_eqL1=""

spatial_prev_dx=""
spatial_prev_ehL2=""
spatial_prev_eqL2=""

spatial_idx=1
for n in $SPATIAL_N_LIST; do
    mesh_init="$TMP_DIR/mesh_${n}.txt"
    create_mesh_init "$n" "$mesh_init"

    echo ""
    echo "Test S${spatial_idx} MUSCL : Maillage N=$n (CFL=$SPATIAL_CFL)"

    out=$(run_solver "$mesh_init" "$SPATIAL_TIME_INIT")
    status=$?
    if [ "$status" -ne 0 ]; then
        echo "Execution echouee pour N=$n"
        echo "$out"
        exit 1
    fi

    dx=$(extract_metric "$out" "Delta x:")
    ehL2=$(extract_metric "$out" "Erreur finale sur la hauteur:")
    eqL2=$(extract_metric "$out" "Erreur finale sur le débit:")
    ehL1=$(extract_metric "$out" "Erreur finale l1(hauteur):")
    eqL1=$(extract_metric "$out" "Erreur finale l1(débit):")
    ehL2m=$(extract_metric "$out" "Erreur finale l2 masquée(hauteur):")
    eqL2m=$(extract_metric "$out" "Erreur finale l2 masquée(débit):")
    ehL1m=$(extract_metric "$out" "Erreur finale l1 masquée(hauteur):")
    eqL1m=$(extract_metric "$out" "Erreur finale l1 masquée(débit):")

    echo "  -> dx=$dx, Erreur h(L2)=$ehL2, Erreur q(L2)=$eqL2, Erreur h(L1)=$ehL1, Erreur q(L1)=$eqL1"
    echo "Spatial_MUSCL,S${spatial_idx},$dx,$SPATIAL_CFL,$ehL2,$eqL2,$ehL1,$eqL1,$ehL2m,$eqL2m,$ehL1m,$eqL1m" >> convergence_results_muscl.csv

    spatial_dxs="$spatial_dxs $dx"
    spatial_ehL2="$spatial_ehL2 $ehL2"
    spatial_eqL2="$spatial_eqL2 $eqL2"
    spatial_ehL1="$spatial_ehL1 $ehL1"
    spatial_eqL1="$spatial_eqL1 $eqL1"

    if [ -n "$spatial_prev_dx" ]; then
        ratio_dx=$(safe_ratio "$spatial_prev_dx" "$dx")
        ratio_h=$(safe_ratio "$spatial_prev_ehL2" "$ehL2")
        ratio_q=$(safe_ratio "$spatial_prev_eqL2" "$eqL2")
        p_h_local=$(safe_order "$ratio_h" "$ratio_dx")
        p_q_local=$(safe_order "$ratio_q" "$ratio_dx")
        echo "     ordre local (L2): h=$p_h_local, q=$p_q_local"
    fi

    spatial_prev_dx="$dx"
    spatial_prev_ehL2="$ehL2"
    spatial_prev_eqL2="$eqL2"
    spatial_idx=$((spatial_idx + 1))
done

ordre_spatial_reg_hL2=$(regression_order "$spatial_dxs" "$spatial_ehL2")
ordre_spatial_reg_qL2=$(regression_order "$spatial_dxs" "$spatial_eqL2")
ordre_spatial_reg_hL1=$(regression_order "$spatial_dxs" "$spatial_ehL1")
ordre_spatial_reg_qL1=$(regression_order "$spatial_dxs" "$spatial_eqL1")

# slope(log(e), log(dx)) should be positive p in e ~ C * dx^p
# regression_order already returns that slope directly.

# ------------------------------
# CAMPAGNE TEMPORELLE (N fixe)
# ------------------------------
echo ""
echo "╔═════════════════════════════════════════════╗"
echo "║   CAMPAGNE MUSCL: CONVERGENCE EN TEMPS      ║"
echo "║   (N fixe, CFL <= 0.45)                     ║"
echo "╚═════════════════════════════════════════════╝"

TEMPORAL_N="2000"
TEMPORAL_CFL_LIST="0.45 0.225 0.1125 0.05625"
TEMPORAL_MESH_INIT="$TMP_DIR/mesh_temporal_${TEMPORAL_N}.txt"
create_mesh_init "$TEMPORAL_N" "$TEMPORAL_MESH_INIT"

temporal_cfls=""
temporal_ehL2=""
temporal_eqL2=""
temporal_ehL1=""
temporal_eqL1=""

temporal_prev_cfl=""
temporal_prev_ehL2=""
temporal_prev_eqL2=""

temporal_idx=1
for cfl in $TEMPORAL_CFL_LIST; do
    time_init="$TMP_DIR/time_temporal_${cfl}.txt"
    create_time_init "$cfl" "$time_init"

    echo ""
    echo "Test T${temporal_idx} MUSCL : Maillage N=$TEMPORAL_N (CFL=$cfl)"

    out=$(run_solver "$TEMPORAL_MESH_INIT" "$time_init")
    status=$?
    if [ "$status" -ne 0 ]; then
        echo "Execution echouee pour CFL=$cfl"
        echo "$out"
        exit 1
    fi

    dx=$(extract_metric "$out" "Delta x:")
    ehL2=$(extract_metric "$out" "Erreur finale sur la hauteur:")
    eqL2=$(extract_metric "$out" "Erreur finale sur le débit:")
    ehL1=$(extract_metric "$out" "Erreur finale l1(hauteur):")
    eqL1=$(extract_metric "$out" "Erreur finale l1(débit):")
    ehL2m=$(extract_metric "$out" "Erreur finale l2 masquée(hauteur):")
    eqL2m=$(extract_metric "$out" "Erreur finale l2 masquée(débit):")
    ehL1m=$(extract_metric "$out" "Erreur finale l1 masquée(hauteur):")
    eqL1m=$(extract_metric "$out" "Erreur finale l1 masquée(débit):")

    echo "  -> dx=$dx, Erreur h(L2)=$ehL2, Erreur q(L2)=$eqL2, Erreur h(L1)=$ehL1, Erreur q(L1)=$eqL1"
    echo "Temporal_MUSCL,T${temporal_idx},$dx,$cfl,$ehL2,$eqL2,$ehL1,$eqL1,$ehL2m,$eqL2m,$ehL1m,$eqL1m" >> convergence_results_muscl.csv

    temporal_cfls="$temporal_cfls $cfl"
    temporal_ehL2="$temporal_ehL2 $ehL2"
    temporal_eqL2="$temporal_eqL2 $eqL2"
    temporal_ehL1="$temporal_ehL1 $ehL1"
    temporal_eqL1="$temporal_eqL1 $eqL1"

    if [ -n "$temporal_prev_cfl" ]; then
        ratio_cfl=$(safe_ratio "$temporal_prev_cfl" "$cfl")
        ratio_h=$(safe_ratio "$temporal_prev_ehL2" "$ehL2")
        ratio_q=$(safe_ratio "$temporal_prev_eqL2" "$eqL2")
        p_h_local=$(safe_order "$ratio_h" "$ratio_cfl")
        p_q_local=$(safe_order "$ratio_q" "$ratio_cfl")
        echo "     ordre local (L2): h=$p_h_local, q=$p_q_local"
    fi

    temporal_prev_cfl="$cfl"
    temporal_prev_ehL2="$ehL2"
    temporal_prev_eqL2="$eqL2"
    temporal_idx=$((temporal_idx + 1))
done

# For fixed N, dt is proportional to CFL, so regression vs CFL gives temporal order
ordre_temporal_reg_hL2=$(regression_order "$temporal_cfls" "$temporal_ehL2")
ordre_temporal_reg_qL2=$(regression_order "$temporal_cfls" "$temporal_eqL2")
ordre_temporal_reg_hL1=$(regression_order "$temporal_cfls" "$temporal_ehL1")
ordre_temporal_reg_qL1=$(regression_order "$temporal_cfls" "$temporal_eqL1")

echo ""
echo "╔═════════════════════════════════════════════╗"
echo "║   RESULTATS CONVERGENCE MUSCL (REGRESSION)  ║"
echo "╚═════════════════════════════════════════════╝"

echo ""
echo "=== ESPACE (regression log-log) ==="
printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-26s │ %-10s │\n" "Ordre spatial MUSCL" "Valeur"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-26s │ %10s │\n" "Hauteur (L2)" "$ordre_spatial_reg_hL2"
printf "│ %-26s │ %10s │\n" "Debit   (L2)" "$ordre_spatial_reg_qL2"
printf "│ %-26s │ %10s │\n" "Hauteur (L1)" "$ordre_spatial_reg_hL1"
printf "│ %-26s │ %10s │\n" "Debit   (L1)" "$ordre_spatial_reg_qL1"
printf "└─────────────────────────────────────────────┘\n"

echo ""
echo "=== TEMPS (regression log-log, CFL <= 0.45) ==="
printf "┌─────────────────────────────────────────────┐\n"
printf "│ %-26s │ %-10s │\n" "Ordre temporel MUSCL" "Valeur"
printf "├─────────────────────────────────────────────┤\n"
printf "│ %-26s │ %10s │\n" "Hauteur (L2)" "$ordre_temporal_reg_hL2"
printf "│ %-26s │ %10s │\n" "Debit   (L2)" "$ordre_temporal_reg_qL2"
printf "│ %-26s │ %10s │\n" "Hauteur (L1)" "$ordre_temporal_reg_hL1"
printf "│ %-26s │ %10s │\n" "Debit   (L1)" "$ordre_temporal_reg_qL1"
printf "└─────────────────────────────────────────────┘\n"

echo ""
echo "Campagne spatiale N : $SPATIAL_N_LIST"
echo "Campagne temporelle CFL : $TEMPORAL_CFL_LIST"
echo "(Respect de la contrainte de stabilite: CFL maximale = 0.45)"
echo ""
echo "Resultats detailles sauvegardes dans convergence_results_muscl.csv"
