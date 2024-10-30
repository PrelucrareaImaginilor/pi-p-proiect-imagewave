ESTIMAREA ADANCIMII IMAGINILOR FOLOSIND TEHNICI STEREO SI RECTIFICARE A IMAGINILOR

1.Analiza literaturii de specialitate

| Nr. | Autor/ An | Titlul articolului/proiectului | Aplicatie/Domeniu | Tehnologii utilizate | Metodologie/Abordare | Rezultate | Limitari |
|---|---|---|---|---|---|---|---|
|1|Qiaochu ZHAO, Ittetsu TANIGUCHI, Takao ONOYE / 2020 |A template-free object motion estimation method for industrial vision system in aligning machine|Industrial aligning machine|Intel Xeon CPU|Magic Line Algorithm, Edge Extraction Algorithm|
|2|Sumit K. Chatterjee, Sravan K. Vittapu/2019|An Efficient Motion Estimation Algorithm for Mobile Video Applications|Video compression|Block matching algorithm| |Average PSNR: 26.90%|
|3|Wenjuan Lu , Huiting Ma , and Daxing Zeng /2024|Performance of a Novel Muscle Synergy Approach for Continuous Motion Estimation on Untrained Motion| Human- robot interaction|Ninapro Database2, 22-sensor CyberGlove II, Bluetooth tunnel serial port,  Trigno Wireless electrodes|NARX neural network| Highest prediction accuracy of 96.3%|Inadequate number of experimental subjects|
|4|Chang Sun, Wenpeng Ren, Jia Su*, Yong Guan /2019|Fast Motion Estimation Method for Self-driving Video|Self-driving cars, video processing, video compression|High Efficiency Video Coding (HEVC), Motion estimation algorithms|DFMD (Fast Prediction Mode Selection), DFMVP (Fast Motion Vector)|Reduce the encoding time by 30%|Small quality loss|
|5|Qinke Yang, Chenming Li, Yuan Li /2022|Human Motion Monitoring and Estimation based on Optical Measurement|Medical, records and analyzes the changes of the main joint angles of the human body|The optical motion capture system Optitrack, Neural Network|Design a neural network model to estimate the amount of motion|The exercise volume score was 73.44%|

**SCHEMA BLOC A PROIECTULUI**

![schemabloc](https://github.com/user-attachments/assets/c57bec87-b6f8-4d8a-b83f-9af64136048a)

**EXPLICATII**

1. Achiziția seturilor de date stereo, care conțin imagini și adnotări de adâncime. Aceste date sunt esențiale pentru a antrena și testa algoritmii de estimare a adâncimii. Seturile de date pot include KITTI, Middlebury, ETH3D etc.
2. Alinierea imaginilor astfel încât liniile orizontale din scena reală să fie păstrate orizontale în ambele imagini. Rectificarea facilitează identificarea punctelor corespondente, permițând calculul precis al disparității.
3. Identificarea punctelor corespondente între cele două imagini. Pentru fiecare pixel din imaginea stângă se caută pixelul corespunzător din imaginea dreaptă, rezultând harta de disparitate, care reprezintă diferențele de poziție ale punctelor între cele două imagini.
4. Generarea hărții de disparitate pe baza datelor de stereo matching. Această hartă oferă informații despre distanțele relative dintre puncte, dar nu încă adâncimea în sine.
5. Conversia hărții de disparitate într-o hartă de adâncime folosind formula de adâncime. Aceasta implică utilizarea ecuației adâncimii pentru a converti valorile de disparitate în adâncimi reale.
6. Afișarea și interpretarea hărții de adâncime rezultată. Acest modul poate include filtre pentru a îmbunătăți claritatea și acuratețea hărții sau evaluarea acesteia comparativ cu datele ground trut (dacă sunt disponibile) pentru a calcula erorile de adâncime.
