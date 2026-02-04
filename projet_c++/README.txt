ce programme calcul la solution numérique d'une rupture de barrage 
avec une méthode de volume fini, en implémentant le schéma de rusanov

les fichiers d'entrée contiennent les paramètres du problème
et une condition initiale de type rupture de barrage

lis de la manière suivante :

fichier d'initialisation du maillage :

  Longeur_gauche longueur_gauche
  Longeur_droite longueur_droite
  Nombre_de_mailles nb_subdivisions
  Hauteur_gauche hgauche
  Hauteur_droite hdroite  

  (vitesse initiale nulle partout)

fichier d'initialisation des conditions initiales :

  temps_initial t_init
  temps_final t_final
  coefficient_cfl coef_cfl
  nom_fichier_sortie output_filename
  nb de sorties voulues int_sortie



exemple de commande pour compiler et exécuter le programme :

  make
  ./build/projet_c++_S7_Debug initialisation_maillage/init_test_maillage.txt initialisation_temps/init_test_temps.txt


la discontinnuité initiale est située exactement en x=0



pour le moment, le system utilise EE en temps, avec des conditions au bords de neumann = 0 ( sortie libre)

pour creer les images et les animations , ulitiser les scripts bash dans le dossier gnuplot 

il faut aussi supprimer les dossiers frames et frames_exacte avant de lancer les scripts pour ne pas avoir d'images residuelles
de meme pour les dossier output et output_exacte


./gnuplot/create_animation.sh premiere_sortie 1      

première_sortie est le nom du fichier de sortie, 1 est le nombre de pas de temps à prendre entre deux sorties, ne pas changer

./gnuplot/create_animation_exacte.sh première_sortie 1













liste : 
-classe fille ppour ordre 2 et 1 
-