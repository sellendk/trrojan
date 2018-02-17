// This file was auto-generated using Create-SphereShaders.ps1 on 02/17/2018 17:02:58
#define SPHERE_TECHNIQUE_USE_GEO (0x80000000)
#define SPHERE_TECHNIQUE_USE_TESS (0x40000000)
#define SPHERE_TECHNIQUE_USE_SRV (0x20000000)
#define SPHERE_TECHNIQUE_USE_RAYCASTING (0x10000000)
#define SPHERE_TECHNIQUE_USE_INSTANCING (0x8000000)
#define SPHERE_INPUT_PV_COLOUR (0x1)
#define SPHERE_INPUT_PV_RADIUS (0x2)
#define SPHERE_INPUT_PV_INTENSITY (0x4)
#define SPHERE_INPUT_PP_INTENSITY (0x10000)
#define SPHERE_INPUT_FLT_COLOUR (0x20000)
#define SPHERE_VARIANT_PV_RAY (0x40000)
#define SPHERE_VARIANT_CONSERVATIVE_DEPTH (0x80000)
#define SPHERE_TECHNIQUE_POLY_TESS (0x1000000050000000ull)
#define SPHERE_TECHNIQUE_ADAPT_HEMISPHERE_TESS (0x10000040000000ull)
#define SPHERE_TECHNIQUE_HEMISPHERE_TESS (0x20000040000000ull)
#define SPHERE_TECHNIQUE_QUAD_TESS (0x2000000050000000ull)
#define SPHERE_TECHNIQUE_ADAPT_SPHERE_TESS (0x40000040000000ull)
#define SPHERE_TECHNIQUE_SPHERE_TESS (0x80000040000000ull)
#define SPHERE_TECHNIQUE_QUAD_INST (0x8000000038000000ull)
#define SPHERE_TECHNIQUE_ADAPT_POLY_TESS (0x800000050000000ull)
#define SPHERE_TECHNIQUE_STPA (0x400000090000000ull)
#define SPHERE_TECHNIQUE_GEO_QUAD (0x200000090000000ull)
#define SPHERE_TECHNIQUE_GEO_POLY (0x100000090000000ull)
#define _DEFINE_SPHERE_TECHNIQUE_LUT(n) static const struct { const char *name; std::uint64_t id; } n[] = {\
{ "poly_tess", 0x1000000050000000ull },\
{ "adapt_hemisphere_tess", 0x10000040000000ull },\
{ "hemisphere_tess", 0x20000040000000ull },\
{ "quad_tess", 0x2000000050000000ull },\
{ "adapt_sphere_tess", 0x40000040000000ull },\
{ "sphere_tess", 0x80000040000000ull },\
{ "quad_inst", 0x8000000038000000ull },\
{ "adapt_poly_tess", 0x800000050000000ull },\
{ "stpa", 0x400000090000000ull },\
{ "geo_quad", 0x200000090000000ull },\
{ "geo_poly", 0x100000090000000ull },\
{ nullptr, 0 } }
#define _ADD_SPHERE_SHADERS(ht)\
ht[36028798092705795] = { 256, 257, 258, 0, 259 };\
ht[9223372037794299908] = { 260, 0, 0, 0, 261 };\
ht[72057596454633474] = { 262, 0, 0, 263, 264 };\
ht[1152921505949548546] = { 265, 266, 267, 0, 268 };\
ht[9223372037795086342] = { 269, 0, 0, 0, 270 };\
ht[9223372037795086340] = { 271, 0, 0, 0, 272 };\
ht[9223372037795086339] = { 273, 0, 0, 0, 274 };\
ht[9223372037795086338] = { 275, 0, 0, 0, 276 };\
ht[9223372037795086337] = { 277, 0, 0, 0, 278 };\
ht[9223372037795086336] = { 279, 0, 0, 0, 280 };\
ht[18014399583289346] = { 281, 282, 283, 0, 284 };\
ht[288230378568417284] = { 285, 0, 0, 286, 287 };\
ht[18014399583289344] = { 288, 289, 290, 0, 291 };\
ht[576460753645600771] = { 292, 293, 294, 0, 295 };\
ht[1152921505949548550] = { 296, 297, 298, 0, 299 };\
ht[576460753646125062] = { 300, 301, 302, 0, 303 };\
ht[9223372037794365442] = { 304, 0, 0, 0, 305 };\
ht[576460753646125060] = { 306, 307, 308, 0, 309 };\
ht[18014399583223814] = { 310, 311, 312, 0, 313 };\
ht[576460753646125058] = { 314, 315, 316, 0, 317 };\
ht[18014399583223812] = { 318, 319, 320, 0, 321 };\
ht[576460753646125056] = { 322, 323, 324, 0, 325 };\
ht[18014399583223810] = { 326, 327, 328, 0, 329 };\
ht[18014399583223809] = { 330, 331, 332, 0, 333 };\
ht[18014399583223808] = { 334, 335, 336, 0, 337 };\
ht[72057596454371332] = { 338, 0, 0, 339, 340 };\
ht[144115190492102658] = { 341, 0, 0, 342, 343 };\
ht[144115190492102656] = { 344, 0, 0, 345, 346 };\
ht[2305843010556198914] = { 347, 348, 349, 0, 350 };\
ht[9223372037794562049] = { 351, 0, 0, 0, 352 };\
ht[2305843010556198912] = { 353, 354, 355, 0, 356 };\
ht[9223372037794955267] = { 357, 0, 0, 0, 358 };\
ht[9223372037794955265] = { 359, 0, 0, 0, 360 };\
ht[288230378568155142] = { 361, 0, 0, 362, 363 };\
ht[72057596454436866] = { 364, 0, 0, 365, 366 };\
ht[72057596454436864] = { 367, 0, 0, 368, 369 };\
ht[288230378567630851] = { 370, 0, 0, 371, 372 };\
ht[1152921505949614080] = { 373, 374, 375, 0, 376 };\
ht[288230378568482818] = { 377, 0, 0, 378, 379 };\
ht[144115190492037126] = { 380, 0, 0, 381, 382 };\
ht[288230378568482816] = { 383, 0, 0, 384, 385 };\
ht[144115190492037124] = { 386, 0, 0, 387, 388 };\
ht[288230378567630854] = { 389, 0, 0, 390, 391 };\
ht[144115190492037122] = { 392, 0, 0, 393, 394 };\
ht[144115190492037121] = { 395, 0, 0, 396, 397 };\
ht[144115190492037120] = { 398, 0, 0, 399, 400 };\
ht[2305843010556133379] = { 401, 402, 403, 0, 404 };\
ht[2305843010556133378] = { 405, 406, 407, 0, 408 };\
ht[2305843010556133377] = { 409, 410, 411, 0, 412 };\
ht[2305843010556133376] = { 413, 414, 415, 0, 416 };\
ht[9223372037794299910] = { 417, 0, 0, 0, 418 };\
ht[72057596454371334] = { 419, 0, 0, 420, 421 };\
ht[1152921505949351938] = { 422, 423, 424, 0, 425 };\
ht[288230378567892996] = { 426, 0, 0, 427, 428 };\
ht[72057596454371331] = { 429, 0, 0, 430, 431 };\
ht[1152921505949548548] = { 432, 433, 434, 0, 435 };\
ht[72057596454371329] = { 436, 0, 0, 437, 438 };\
ht[72057596454371328] = { 439, 0, 0, 440, 441 };\
ht[1152921505949548545] = { 442, 443, 444, 0, 445 };\
ht[1152921505949548544] = { 446, 447, 448, 0, 449 };\
ht[288230378568417282] = { 450, 0, 0, 451, 452 };\
ht[288230378568417281] = { 453, 0, 0, 454, 455 };\
ht[288230378568417280] = { 456, 0, 0, 457, 458 };\
ht[1152921505949024262] = { 459, 460, 461, 0, 462 };\
ht[9223372037794299907] = { 463, 0, 0, 0, 464 };\
ht[144115190492561411] = { 465, 0, 0, 466, 467 };\
ht[9223372037794824198] = { 468, 0, 0, 0, 469 };\
ht[9223372037794824196] = { 470, 0, 0, 0, 471 };\
ht[9223372037794824195] = { 472, 0, 0, 0, 473 };\
ht[9223372037794824194] = { 474, 0, 0, 0, 475 };\
ht[9223372037794824193] = { 476, 0, 0, 0, 477 };\
ht[9223372037794824192] = { 478, 0, 0, 0, 479 };\
ht[288230378568155139] = { 480, 0, 0, 481, 482 };\
ht[576460753645600770] = { 483, 484, 485, 0, 486 };\
ht[576460753645862918] = { 487, 488, 489, 0, 490 };\
ht[576460753645666306] = { 491, 492, 493, 0, 494 };\
ht[576460753645862916] = { 495, 496, 497, 0, 498 };\
ht[576460753645862915] = { 499, 500, 501, 0, 502 };\
ht[576460753645862914] = { 503, 504, 505, 0, 506 };\
ht[576460753645862913] = { 507, 508, 509, 0, 510 };\
ht[576460753645862912] = { 511, 512, 513, 0, 514 };\
ht[1152921505949286406] = { 515, 516, 517, 0, 518 };\
ht[2305843010556395526] = { 519, 520, 521, 0, 522 };\
ht[1152921505949089794] = { 523, 524, 525, 0, 526 };\
ht[9223372037795151874] = { 527, 0, 0, 0, 528 };\
ht[9223372037794693121] = { 529, 0, 0, 0, 530 };\
ht[144115190491840514] = { 531, 0, 0, 532, 533 };\
ht[144115190491840512] = { 534, 0, 0, 535, 536 };\
ht[2305843010555936770] = { 537, 538, 539, 0, 540 };\
ht[72057596453912578] = { 541, 0, 0, 542, 543 };\
ht[2305843010555936768] = { 544, 545, 546, 0, 547 };\
ht[1152921505949286403] = { 548, 549, 550, 0, 551 };\
ht[576460753646125057] = { 552, 553, 554, 0, 555 };\
ht[9223372037794693123] = { 556, 0, 0, 0, 557 };\
ht[576460753645928450] = { 558, 559, 560, 0, 561 };\
ht[9223372037794562048] = { 562, 0, 0, 0, 563 };\
ht[576460753646190592] = { 564, 565, 566, 0, 567 };\
ht[2305843010556133380] = { 568, 569, 570, 0, 571 };\
ht[72057596454174720] = { 572, 0, 0, 573, 574 };\
ht[1152921505949351936] = { 575, 576, 577, 0, 578 };\
ht[288230378568220674] = { 579, 0, 0, 580, 581 };\
ht[144115190491774982] = { 582, 0, 0, 583, 584 };\
ht[288230378568220672] = { 585, 0, 0, 586, 587 };\
ht[144115190491774980] = { 588, 0, 0, 589, 590 };\
ht[144115190491774979] = { 591, 0, 0, 592, 593 };\
ht[144115190491774978] = { 594, 0, 0, 595, 596 };\
ht[144115190491774977] = { 597, 0, 0, 598, 599 };\
ht[144115190491774976] = { 600, 0, 0, 601, 602 };\
ht[2305843010555871235] = { 603, 604, 605, 0, 606 };\
ht[2305843010555871234] = { 607, 608, 609, 0, 610 };\
ht[2305843010555871233] = { 611, 612, 613, 0, 614 };\
ht[2305843010555871232] = { 615, 616, 617, 0, 618 };\
ht[72057596454109190] = { 619, 0, 0, 620, 621 };\
ht[9223372037794627584] = { 622, 0, 0, 0, 623 };\
ht[72057596454109187] = { 624, 0, 0, 625, 626 };\
ht[1152921505949286404] = { 627, 628, 629, 0, 630 };\
ht[72057596454109185] = { 631, 0, 0, 632, 633 };\
ht[1152921505949810690] = { 634, 635, 636, 0, 637 };\
ht[288230378568155140] = { 638, 0, 0, 639, 640 };\
ht[1152921505949286400] = { 641, 642, 643, 0, 644 };\
ht[288230378568155138] = { 645, 0, 0, 646, 647 };\
ht[288230378568155137] = { 648, 0, 0, 649, 650 };\
ht[288230378568155136] = { 651, 0, 0, 652, 653 };\
ht[1152921505949810691] = { 654, 655, 656, 0, 657 };\
ht[9223372037794562054] = { 658, 0, 0, 0, 659 };\
ht[9223372037794562052] = { 660, 0, 0, 0, 661 };\
ht[36028798092771330] = { 662, 663, 664, 0, 665 };\
ht[9223372037794562050] = { 666, 0, 0, 0, 667 };\
ht[36028798092771328] = { 668, 669, 670, 0, 671 };\
ht[576460753645666304] = { 672, 673, 674, 0, 675 };\
ht[1152921505949810694] = { 676, 677, 678, 0, 679 };\
ht[9223372037795217411] = { 680, 0, 0, 0, 681 };\
ht[9223372037794627586] = { 682, 0, 0, 0, 683 };\
ht[576460753645600769] = { 684, 685, 686, 0, 687 };\
ht[576460753645600774] = { 688, 689, 690, 0, 691 };\
ht[36028798092705796] = { 692, 693, 694, 0, 695 };\
ht[576460753645600772] = { 696, 697, 698, 0, 699 };\
ht[36028798092705794] = { 700, 701, 702, 0, 703 };\
ht[36028798092705793] = { 704, 705, 706, 0, 707 };\
ht[36028798092705792] = { 708, 709, 710, 0, 711 };\
ht[576460753645600768] = { 712, 713, 714, 0, 715 };\
ht[288230378567892995] = { 716, 0, 0, 717, 718 };\
ht[9223372037794365440] = { 719, 0, 0, 0, 720 };\
ht[2305843010555871238] = { 721, 722, 723, 0, 724 };\
ht[288230378568417286] = { 725, 0, 0, 726, 727 };\
ht[1152921505949876226] = { 728, 729, 730, 0, 731 };\
ht[9007200328482822] = { 732, 733, 734, 0, 735 };\
ht[9223372037794889728] = { 736, 0, 0, 0, 737 };\
ht[9223372037794430979] = { 738, 0, 0, 0, 739 };\
ht[72057596454371330] = { 740, 0, 0, 741, 742 };\
ht[9223372037794430977] = { 743, 0, 0, 0, 744 };\
ht[288230378568417283] = { 745, 0, 0, 746, 747 };\
ht[1152921505949286402] = { 748, 749, 750, 0, 751 };\
ht[72057596453912576] = { 752, 0, 0, 753, 754 };\
ht[1152921505949089792] = { 755, 756, 757, 0, 758 };\
ht[288230378567958530] = { 759, 0, 0, 760, 761 };\
ht[288230378567958528] = { 762, 0, 0, 763, 764 };\
ht[1152921505949614082] = { 765, 766, 767, 0, 768 };\
ht[2305843010556461058] = { 769, 770, 771, 0, 772 };\
ht[288230378567630850] = { 773, 0, 0, 774, 775 };\
ht[36028798092705798] = { 776, 777, 778, 0, 779 };\
ht[72057596454109184] = { 780, 0, 0, 781, 782 };\
ht[18014399583223811] = { 783, 784, 785, 0, 786 };\
ht[72057596453847044] = { 787, 0, 0, 788, 789 };\
ht[72057596453847043] = { 790, 0, 0, 791, 792 };\
ht[1152921505949024260] = { 793, 794, 795, 0, 796 };\
ht[72057596453847041] = { 797, 0, 0, 798, 799 };\
ht[72057596453847040] = { 800, 0, 0, 801, 802 };\
ht[1152921505949024257] = { 803, 804, 805, 0, 806 };\
ht[1152921505949024256] = { 807, 808, 809, 0, 810 };\
ht[288230378567892994] = { 811, 0, 0, 812, 813 };\
ht[288230378567892993] = { 814, 0, 0, 815, 816 };\
ht[288230378567892992] = { 817, 0, 0, 818, 819 };\
ht[144115190492626946] = { 820, 0, 0, 821, 822 };\
ht[144115190492626944] = { 823, 0, 0, 824, 825 };\
ht[2305843010556723202] = { 826, 827, 828, 0, 829 };\
ht[144115190492299265] = { 830, 0, 0, 831, 832 };\
ht[2305843010556723200] = { 833, 834, 835, 0, 836 };\
ht[2305843010556657668] = { 837, 838, 839, 0, 840 };\
ht[72057596454109188] = { 841, 0, 0, 842, 843 };\
ht[1152921505949024258] = { 844, 845, 846, 0, 847 };\
ht[9223372037794299906] = { 848, 0, 0, 0, 849 };\
ht[9223372037794299905] = { 850, 0, 0, 0, 851 };\
ht[9223372037794299904] = { 852, 0, 0, 0, 853 };\
ht[4503600701177858] = { 854, 855, 856, 0, 857 };\
ht[4503600701177856] = { 858, 859, 860, 0, 861 };\
ht[144115190492561414] = { 862, 0, 0, 863, 864 };\
ht[144115190492561412] = { 865, 0, 0, 866, 867 };\
ht[576460753646190594] = { 868, 869, 870, 0, 871 };\
ht[144115190492561410] = { 872, 0, 0, 873, 874 };\
ht[144115190492561409] = { 875, 0, 0, 876, 877 };\
ht[144115190492561408] = { 878, 0, 0, 879, 880 };\
ht[2305843010556657667] = { 881, 882, 883, 0, 884 };\
ht[2305843010556657666] = { 885, 886, 887, 0, 888 };\
ht[2305843010556657665] = { 889, 890, 891, 0, 892 };\
ht[2305843010556657664] = { 893, 894, 895, 0, 896 };\
ht[2305843010556657670] = { 897, 898, 899, 0, 900 };\
ht[72057596453847042] = { 901, 0, 0, 902, 903 };\
ht[4503600701112326] = { 904, 905, 906, 0, 907 };\
ht[4503600701112324] = { 908, 909, 910, 0, 911 };\
ht[4503600701112323] = { 912, 913, 914, 0, 915 };\
ht[4503600701112322] = { 916, 917, 918, 0, 919 };\
ht[4503600701112321] = { 920, 921, 922, 0, 923 };\
ht[4503600701112320] = { 924, 925, 926, 0, 927 };\
ht[72057596454174722] = { 928, 0, 0, 929, 930 };\
ht[1152921505949548547] = { 931, 932, 933, 0, 934 };\
ht[576460753645928448] = { 935, 936, 937, 0, 938 };\
ht[144115190492037123] = { 939, 0, 0, 940, 941 };\
ht[72057596453847046] = { 942, 0, 0, 943, 944 };\
ht[288230378567892998] = { 945, 0, 0, 946, 947 };\
ht[576460753646452738] = { 948, 949, 950, 0, 951 };\
ht[9223372037795151872] = { 952, 0, 0, 0, 953 };\
ht[576460753646452736] = { 954, 955, 956, 0, 957 };\
ht[9007200328548354] = { 958, 959, 960, 0, 961 };\
ht[9007200328548352] = { 962, 963, 964, 0, 965 };\
ht[288230378567696386] = { 966, 0, 0, 967, 968 };\
ht[1152921505949286401] = { 969, 970, 971, 0, 972 };\
ht[288230378567696384] = { 973, 0, 0, 974, 975 };\
ht[9223372037794562051] = { 976, 0, 0, 0, 977 };\
ht[576460753646387206] = { 978, 979, 980, 0, 981 };\
ht[576460753646387204] = { 982, 983, 984, 0, 985 };\
ht[576460753646387203] = { 986, 987, 988, 0, 989 };\
ht[576460753646387202] = { 990, 991, 992, 0, 993 };\
ht[576460753646387201] = { 994, 995, 996, 0, 997 };\
ht[576460753646387200] = { 998, 999, 1000, 0, 1001 };\
ht[9007200328482820] = { 1002, 1003, 1004, 0, 1005 };\
ht[9007200328482819] = { 1006, 1007, 1008, 0, 1009 };\
ht[9007200328482818] = { 1010, 1011, 1012, 0, 1013 };\
ht[9007200328482817] = { 1014, 1015, 1016, 0, 1017 };\
ht[9007200328482816] = { 1018, 1019, 1020, 0, 1021 };\
ht[288230378567630852] = { 1022, 0, 0, 1023, 1024 };\
ht[576460753646125059] = { 1025, 1026, 1027, 0, 1028 };\
ht[144115190492299264] = { 1029, 0, 0, 1030, 1031 };\
ht[288230378567630849] = { 1032, 0, 0, 1033, 1034 };\
ht[288230378567630848] = { 1035, 0, 0, 1036, 1037 };\
ht[144115190492364802] = { 1038, 0, 0, 1039, 1040 };\
ht[2305843010556133382] = { 1041, 1042, 1043, 0, 1044 };\
ht[144115190492364800] = { 1045, 0, 0, 1046, 1047 };\
ht[72057596454109186] = { 1048, 0, 0, 1049, 1050 };\
ht[2305843010556461056] = { 1051, 1052, 1053, 0, 1054 };\
ht[9223372037794889730] = { 1055, 0, 0, 0, 1056 };\
ht[9223372037795217409] = { 1057, 0, 0, 0, 1058 };\
ht[72057596454699010] = { 1059, 0, 0, 1060, 1061 };\
ht[72057596454699008] = { 1062, 0, 0, 1063, 1064 };\
ht[1152921505949876224] = { 1065, 1066, 1067, 0, 1068 };\
ht[1152921505949024259] = { 1069, 1070, 1071, 0, 1072 };\
ht[144115190492299270] = { 1073, 0, 0, 1074, 1075 };\
ht[144115190492299268] = { 1076, 0, 0, 1077, 1078 };\
ht[144115190492299267] = { 1079, 0, 0, 1080, 1081 };\
ht[144115190492299266] = { 1082, 0, 0, 1083, 1084 };\
ht[2305843010555871236] = { 1085, 1086, 1087, 0, 1088 };\
ht[2305843010556395524] = { 1089, 1090, 1091, 0, 1092 };\
ht[2305843010556395523] = { 1093, 1094, 1095, 0, 1096 };\
ht[2305843010556395522] = { 1097, 1098, 1099, 0, 1100 };\
ht[2305843010556395521] = { 1101, 1102, 1103, 0, 1104 };\
ht[2305843010556395520] = { 1105, 1106, 1107, 0, 1108 };\
ht[72057596454633478] = { 1109, 0, 0, 1110, 1111 };\
ht[72057596454633476] = { 1112, 0, 0, 1113, 1114 };\
ht[72057596454633475] = { 1115, 0, 0, 1116, 1117 };\
ht[1152921505949810692] = { 1118, 1119, 1120, 0, 1121 };\
ht[72057596454633473] = { 1122, 0, 0, 1123, 1124 };\
ht[72057596454633472] = { 1125, 0, 0, 1126, 1127 };\
ht[1152921505949810689] = { 1128, 1129, 1130, 0, 1131 };\
ht[1152921505949810688] = { 1132, 1133, 1134, 0, 1135 }
