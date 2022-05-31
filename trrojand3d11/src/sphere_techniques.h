// This file was auto-generated by ".\Create-SphereShaders.ps1 -OutPath . -ResourceStart 1000" on 05/31/2022 14:40:39
#define SPHERE_TECHNIQUE_USE_GEO (0x80000000)
#define SPHERE_TECHNIQUE_USE_TESS (0x40000000)
#define SPHERE_TECHNIQUE_USE_SRV (0x20000000)
#define SPHERE_TECHNIQUE_USE_RAYCASTING (0x10000000)
#define SPHERE_TECHNIQUE_USE_INSTANCING (0x8000000)
#define SPHERE_INPUT_PV_COLOUR (0x1)
#define SPHERE_INPUT_PV_RADIUS (0x2)
#define SPHERE_INPUT_PV_INTENSITY (0x4)
#define SPHERE_INPUT_PP_INTENSITY (0x100)
#define SPHERE_INPUT_FLT_COLOUR (0x8)
#define SPHERE_VARIANT_PV_RAY (0x200)
#define SPHERE_VARIANT_CONSERVATIVE_DEPTH (0x400)
#define SPHERE_TECHNIQUE_RESERVED_MMPLD (0x4000000)
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
ht[9223372037794299908] = { 1000, 0, 0, 0, 1001 };\
ht[9007200328482816] = { 1002, 1003, 1004, 0, 1005 };\
ht[144115190491776006] = { 1006, 0, 0, 1007, 1008 };\
ht[144115190491776004] = { 1009, 0, 0, 1010, 1011 };\
ht[144115190491776003] = { 1012, 0, 0, 1013, 1014 };\
ht[144115190491776002] = { 1015, 0, 0, 1016, 1017 };\
ht[144115190491776001] = { 1018, 0, 0, 1019, 1020 };\
ht[144115190491776000] = { 1021, 0, 0, 1022, 1023 };\
ht[2305843010555872259] = { 1024, 1025, 1026, 0, 1027 };\
ht[2305843010555872258] = { 1028, 1029, 1030, 0, 1031 };\
ht[2305843010555872257] = { 1032, 1033, 1034, 0, 1035 };\
ht[2305843010555872256] = { 1036, 1037, 1038, 0, 1039 };\
ht[288230378567631618] = { 1040, 0, 0, 1041, 1042 };\
ht[576460753645600771] = { 1043, 1044, 1045, 0, 1046 };\
ht[72057596453848834] = { 1047, 0, 0, 1048, 1049 };\
ht[2305843010555872260] = { 1050, 1051, 1052, 0, 1053 };\
ht[72057596453847296] = { 1054, 0, 0, 1055, 1056 };\
ht[1152921505949025793] = { 1057, 1058, 1059, 0, 1060 };\
ht[72057596453848582] = { 1061, 0, 0, 1062, 1063 };\
ht[72057596453848580] = { 1064, 0, 0, 1065, 1066 };\
ht[72057596453848579] = { 1067, 0, 0, 1068, 1069 };\
ht[18014399583223812] = { 1070, 1071, 1072, 0, 1073 };\
ht[72057596453848577] = { 1074, 0, 0, 1075, 1076 };\
ht[1152921505949025794] = { 1077, 1078, 1079, 0, 1080 };\
ht[18014399583223809] = { 1081, 1082, 1083, 0, 1084 };\
ht[1152921505949025792] = { 1085, 1086, 1087, 0, 1088 };\
ht[9223372037794300162] = { 1089, 0, 0, 0, 1090 };\
ht[1152921505949026050] = { 1091, 1092, 1093, 0, 1094 };\
ht[9223372037794300160] = { 1095, 0, 0, 0, 1096 };\
ht[576460753645601538] = { 1097, 1098, 1099, 0, 1100 };\
ht[9223372037794301451] = { 1101, 0, 0, 0, 1102 };\
ht[9223372037794301449] = { 1103, 0, 0, 0, 1104 };\
ht[288230378567631875] = { 1105, 0, 0, 1106, 1107 };\
ht[9223372037794301446] = { 1108, 0, 0, 0, 1109 };\
ht[72057596453848322] = { 1110, 0, 0, 1111, 1112 };\
ht[9223372037794301444] = { 1113, 0, 0, 0, 1114 };\
ht[9223372037794301443] = { 1115, 0, 0, 0, 1116 };\
ht[9223372037794301442] = { 1117, 0, 0, 0, 1118 };\
ht[9223372037794301441] = { 1119, 0, 0, 0, 1120 };\
ht[9223372037794301440] = { 1121, 0, 0, 0, 1122 };\
ht[576460753645601798] = { 1123, 1124, 1125, 0, 1126 };\
ht[288230378567632385] = { 1127, 0, 0, 1128, 1129 };\
ht[576460753645601796] = { 1130, 1131, 1132, 0, 1133 };\
ht[576460753645601795] = { 1134, 1135, 1136, 0, 1137 };\
ht[288230378567631872] = { 1138, 0, 0, 1139, 1140 };\
ht[576460753645601793] = { 1141, 1142, 1143, 0, 1144 };\
ht[576460753645601792] = { 1145, 1146, 1147, 0, 1148 };\
ht[4503600701112578] = { 1149, 1150, 1151, 0, 1152 };\
ht[4503600701112576] = { 1153, 1154, 1155, 0, 1156 };\
ht[2305843010555872514] = { 1157, 1158, 1159, 0, 1160 };\
ht[288230378567632642] = { 1161, 0, 0, 1162, 1163 };\
ht[2305843010555871235] = { 1164, 1165, 1166, 0, 1167 };\
ht[144115190491775494] = { 1168, 0, 0, 1169, 1170 };\
ht[144115190491775492] = { 1171, 0, 0, 1172, 1173 };\
ht[144115190491775491] = { 1174, 0, 0, 1175, 1176 };\
ht[144115190491775490] = { 1177, 0, 0, 1178, 1179 };\
ht[144115190491775489] = { 1180, 0, 0, 1181, 1182 };\
ht[144115190491775488] = { 1183, 0, 0, 1184, 1185 };\
ht[2305843010555871747] = { 1186, 1187, 1188, 0, 1189 };\
ht[2305843010555871746] = { 1190, 1191, 1192, 0, 1193 };\
ht[2305843010555871745] = { 1194, 1195, 1196, 0, 1197 };\
ht[2305843010555871744] = { 1198, 1199, 1200, 0, 1201 };\
ht[1152921505949024259] = { 1202, 1203, 1204, 0, 1205 };\
ht[1152921505949024262] = { 1206, 1207, 1208, 0, 1209 };\
ht[72057596453847043] = { 1210, 0, 0, 1211, 1212 };\
ht[9223372037794299907] = { 1213, 0, 0, 0, 1214 };\
ht[144115190491776770] = { 1215, 0, 0, 1216, 1217 };\
ht[144115190491776768] = { 1218, 0, 0, 1219, 1220 };\
ht[2305843010555873026] = { 1221, 1222, 1223, 0, 1224 };\
ht[2305843010555873024] = { 1225, 1226, 1227, 0, 1228 };\
ht[288230378567630850] = { 1229, 0, 0, 1230, 1231 };\
ht[9007200328483072] = { 1232, 1233, 1234, 0, 1235 };\
ht[72057596453848070] = { 1236, 0, 0, 1237, 1238 };\
ht[72057596453848068] = { 1239, 0, 0, 1240, 1241 };\
ht[72057596453848067] = { 1242, 0, 0, 1243, 1244 };\
ht[1152921505949025284] = { 1245, 1246, 1247, 0, 1248 };\
ht[1152921505949025282] = { 1249, 1250, 1251, 0, 1252 };\
ht[72057596453848064] = { 1253, 0, 0, 1254, 1255 };\
ht[1152921505949025281] = { 1256, 1257, 1258, 0, 1259 };\
ht[1152921505949025280] = { 1260, 1261, 1262, 0, 1263 };\
ht[288230378567631106] = { 1264, 0, 0, 1265, 1266 };\
ht[288230378567631104] = { 1267, 0, 0, 1268, 1269 };\
ht[72057596453847554] = { 1270, 0, 0, 1271, 1272 };\
ht[9223372037794300939] = { 1273, 0, 0, 0, 1274 };\
ht[9223372037794300937] = { 1275, 0, 0, 0, 1276 };\
ht[288230378567630854] = { 1277, 0, 0, 1278, 1279 };\
ht[9223372037794300934] = { 1280, 0, 0, 0, 1281 };\
ht[9223372037794300932] = { 1282, 0, 0, 0, 1283 };\
ht[288230378567631878] = { 1284, 0, 0, 1285, 1286 };\
ht[9223372037794300930] = { 1287, 0, 0, 0, 1288 };\
ht[9223372037794300929] = { 1289, 0, 0, 0, 1290 };\
ht[9223372037794300928] = { 1291, 0, 0, 0, 1292 };\
ht[1152921505949025286] = { 1293, 1294, 1295, 0, 1296 };\
ht[576460753645601286] = { 1297, 1298, 1299, 0, 1300 };\
ht[576460753645601284] = { 1301, 1302, 1303, 0, 1304 };\
ht[9223372037794300931] = { 1305, 0, 0, 0, 1306 };\
ht[576460753645601282] = { 1307, 1308, 1309, 0, 1310 };\
ht[576460753645601281] = { 1311, 1312, 1313, 0, 1314 };\
ht[576460753645601280] = { 1315, 1316, 1317, 0, 1318 };\
ht[36028798092705795] = { 1319, 1320, 1321, 0, 1322 };\
ht[1152921505949025796] = { 1323, 1324, 1325, 0, 1326 };\
ht[288230378567631874] = { 1327, 0, 0, 1328, 1329 };\
ht[144115190491774979] = { 1330, 0, 0, 1331, 1332 };\
ht[576460753645602562] = { 1333, 1334, 1335, 0, 1336 };\
ht[576460753645602560] = { 1337, 1338, 1339, 0, 1340 };\
ht[288230378567632384] = { 1341, 0, 0, 1342, 1343 };\
ht[144115190491774980] = { 1344, 0, 0, 1345, 1346 };\
ht[1152921505949025026] = { 1347, 1348, 1349, 0, 1350 };\
ht[144115190491774978] = { 1351, 0, 0, 1352, 1353 };\
ht[144115190491774977] = { 1354, 0, 0, 1355, 1356 };\
ht[144115190491774976] = { 1357, 0, 0, 1358, 1359 };\
ht[72057596453848578] = { 1360, 0, 0, 1361, 1362 };\
ht[2305843010555871234] = { 1363, 1364, 1365, 0, 1366 };\
ht[2305843010555871233] = { 1367, 1368, 1369, 0, 1370 };\
ht[2305843010555871232] = { 1371, 1372, 1373, 0, 1374 };\
ht[72057596453848066] = { 1375, 0, 0, 1376, 1377 };\
ht[144115190491776256] = { 1378, 0, 0, 1379, 1380 };\
ht[2305843010555872774] = { 1381, 1382, 1383, 0, 1384 };\
ht[288230378567632388] = { 1385, 0, 0, 1386, 1387 };\
ht[2305843010555872512] = { 1388, 1389, 1390, 0, 1391 };\
ht[1152921505949024514] = { 1392, 1393, 1394, 0, 1395 };\
ht[72057596453847558] = { 1396, 0, 0, 1397, 1398 };\
ht[72057596453847556] = { 1399, 0, 0, 1400, 1401 };\
ht[72057596453847555] = { 1402, 0, 0, 1403, 1404 };\
ht[1152921505949024772] = { 1405, 1406, 1407, 0, 1408 };\
ht[72057596453847553] = { 1409, 0, 0, 1410, 1411 };\
ht[72057596453847552] = { 1412, 0, 0, 1413, 1414 };\
ht[1152921505949024769] = { 1415, 1416, 1417, 0, 1418 };\
ht[1152921505949024768] = { 1419, 1420, 1421, 0, 1422 };\
ht[18014399583224066] = { 1423, 1424, 1425, 0, 1426 };\
ht[2305843010555871238] = { 1427, 1428, 1429, 0, 1430 };\
ht[9223372037794300427] = { 1431, 0, 0, 0, 1432 };\
ht[1152921505949024771] = { 1433, 1434, 1435, 0, 1436 };\
ht[9223372037794300425] = { 1437, 0, 0, 0, 1438 };\
ht[72057596453848832] = { 1439, 0, 0, 1440, 1441 };\
ht[9223372037794300422] = { 1442, 0, 0, 0, 1443 };\
ht[1152921505949026048] = { 1444, 1445, 1446, 0, 1447 };\
ht[2305843010555871748] = { 1448, 1449, 1450, 0, 1451 };\
ht[9223372037794300419] = { 1452, 0, 0, 0, 1453 };\
ht[9223372037794300418] = { 1454, 0, 0, 0, 1455 };\
ht[9223372037794300417] = { 1456, 0, 0, 0, 1457 };\
ht[9223372037794300416] = { 1458, 0, 0, 0, 1459 };\
ht[576460753645600769] = { 1460, 1461, 1462, 0, 1463 };\
ht[576460753645600774] = { 1464, 1465, 1466, 0, 1467 };\
ht[36028798092705796] = { 1468, 1469, 1470, 0, 1471 };\
ht[576460753645600772] = { 1472, 1473, 1474, 0, 1475 };\
ht[9007200328482818] = { 1476, 1477, 1478, 0, 1479 };\
ht[36028798092705793] = { 1480, 1481, 1482, 0, 1483 };\
ht[36028798092705792] = { 1484, 1485, 1486, 0, 1487 };\
ht[576460753645600768] = { 1488, 1489, 1490, 0, 1491 };\
ht[9223372037794301698] = { 1492, 0, 0, 0, 1493 };\
ht[9007200328482817] = { 1494, 1495, 1496, 0, 1497 };\
ht[9223372037794301696] = { 1498, 0, 0, 0, 1499 };\
ht[9223372037794300420] = { 1500, 0, 0, 0, 1501 };\
ht[1152921505949025538] = { 1502, 1503, 1504, 0, 1505 };\
ht[288230378567631616] = { 1506, 0, 0, 1507, 1508 };\
ht[576460753645602050] = { 1509, 1510, 1511, 0, 1512 };\
ht[576460753645602048] = { 1513, 1514, 1515, 0, 1516 };\
ht[1152921505949025283] = { 1517, 1518, 1519, 0, 1520 };\
ht[288230378567631363] = { 1521, 0, 0, 1522, 1523 };\
ht[72057596453847810] = { 1524, 0, 0, 1525, 1526 };\
ht[2305843010555872262] = { 1527, 1528, 1529, 0, 1530 };\
ht[288230378567632640] = { 1531, 0, 0, 1532, 1533 };\
ht[1152921505949025795] = { 1534, 1535, 1536, 0, 1537 };\
ht[288230378567631873] = { 1538, 0, 0, 1539, 1540 };\
ht[2305843010555872772] = { 1541, 1542, 1543, 0, 1544 };\
ht[288230378567632390] = { 1545, 0, 0, 1546, 1547 };\
ht[144115190491775746] = { 1548, 0, 0, 1549, 1550 };\
ht[18014399583223808] = { 1551, 1552, 1553, 0, 1554 };\
ht[144115190491775744] = { 1555, 0, 0, 1556, 1557 };\
ht[2305843010555872002] = { 1558, 1559, 1560, 0, 1561 };\
ht[2305843010555872000] = { 1562, 1563, 1564, 0, 1565 };\
ht[18014399583224064] = { 1566, 1567, 1568, 0, 1569 };\
ht[288230378567632130] = { 1570, 0, 0, 1571, 1572 };\
ht[36028798092705798] = { 1573, 1574, 1575, 0, 1576 };\
ht[72057596453847046] = { 1577, 0, 0, 1578, 1579 };\
ht[18014399583223811] = { 1580, 1581, 1582, 0, 1583 };\
ht[72057596453847044] = { 1584, 0, 0, 1585, 1586 };\
ht[36028798092705794] = { 1587, 1588, 1589, 0, 1590 };\
ht[1152921505949024260] = { 1591, 1592, 1593, 0, 1594 };\
ht[72057596453847041] = { 1595, 0, 0, 1596, 1597 };\
ht[72057596453847040] = { 1598, 0, 0, 1599, 1600 };\
ht[1152921505949024257] = { 1601, 1602, 1603, 0, 1604 };\
ht[1152921505949024256] = { 1605, 1606, 1607, 0, 1608 };\
ht[18014399583223810] = { 1609, 1610, 1611, 0, 1612 };\
ht[2305843010555871750] = { 1613, 1614, 1615, 0, 1616 };\
ht[288230378567632387] = { 1617, 0, 0, 1618, 1619 };\
ht[9223372037794299915] = { 1620, 0, 0, 0, 1621 };\
ht[1152921505949025798] = { 1622, 1623, 1624, 0, 1625 };\
ht[9223372037794299913] = { 1626, 0, 0, 0, 1627 };\
ht[72057596453848320] = { 1628, 0, 0, 1629, 1630 };\
ht[9223372037794299910] = { 1631, 0, 0, 0, 1632 };\
ht[1152921505949025536] = { 1633, 1634, 1635, 0, 1636 };\
ht[288230378567631364] = { 1637, 0, 0, 1638, 1639 };\
ht[1152921505949024258] = { 1640, 1641, 1642, 0, 1643 };\
ht[9223372037794299906] = { 1644, 0, 0, 0, 1645 };\
ht[288230378567631361] = { 1646, 0, 0, 1647, 1648 };\
ht[288230378567631360] = { 1649, 0, 0, 1650, 1651 };\
ht[9223372037794299905] = { 1652, 0, 0, 0, 1653 };\
ht[1152921505949024770] = { 1654, 1655, 1656, 0, 1657 };\
ht[9223372037794301184] = { 1658, 0, 0, 0, 1659 };\
ht[72057596453847042] = { 1660, 0, 0, 1661, 1662 };\
ht[576460753645601536] = { 1663, 1664, 1665, 0, 1666 };\
ht[4503600701112326] = { 1667, 1668, 1669, 0, 1670 };\
ht[4503600701112324] = { 1671, 1672, 1673, 0, 1674 };\
ht[4503600701112323] = { 1675, 1676, 1677, 0, 1678 };\
ht[4503600701112322] = { 1679, 1680, 1681, 0, 1682 };\
ht[4503600701112321] = { 1683, 1684, 1685, 0, 1686 };\
ht[4503600701112320] = { 1687, 1688, 1689, 0, 1690 };\
ht[9223372037794300672] = { 1691, 0, 0, 0, 1692 };\
ht[1152921505949024774] = { 1693, 1694, 1695, 0, 1696 };\
ht[576460753645600770] = { 1697, 1698, 1699, 0, 1700 };\
ht[144115190491775234] = { 1701, 0, 0, 1702, 1703 };\
ht[9007200328483074] = { 1704, 1705, 1706, 0, 1707 };\
ht[144115190491775232] = { 1708, 0, 0, 1709, 1710 };\
ht[288230378567631362] = { 1711, 0, 0, 1712, 1713 };\
ht[2305843010555871490] = { 1714, 1715, 1716, 0, 1717 };\
ht[2305843010555871488] = { 1718, 1719, 1720, 0, 1721 };\
ht[144115190491776518] = { 1722, 0, 0, 1723, 1724 };\
ht[144115190491776516] = { 1725, 0, 0, 1726, 1727 };\
ht[144115190491776515] = { 1728, 0, 0, 1729, 1730 };\
ht[144115190491776514] = { 1731, 0, 0, 1732, 1733 };\
ht[144115190491776513] = { 1734, 0, 0, 1735, 1736 };\
ht[144115190491776512] = { 1737, 0, 0, 1738, 1739 };\
ht[2305843010555872771] = { 1740, 1741, 1742, 0, 1743 };\
ht[2305843010555872770] = { 1744, 1745, 1746, 0, 1747 };\
ht[2305843010555872769] = { 1748, 1749, 1750, 0, 1751 };\
ht[2305843010555872768] = { 1752, 1753, 1754, 0, 1755 };\
ht[576460753645601794] = { 1756, 1757, 1758, 0, 1759 };\
ht[9223372037794301186] = { 1760, 0, 0, 0, 1761 };\
ht[576460753645601283] = { 1762, 1763, 1764, 0, 1765 };\
ht[288230378567631366] = { 1766, 0, 0, 1767, 1768 };\
ht[36028798092706050] = { 1769, 1770, 1771, 0, 1772 };\
ht[9007200328482822] = { 1773, 1774, 1775, 0, 1776 };\
ht[9007200328482820] = { 1777, 1778, 1779, 0, 1780 };\
ht[9007200328482819] = { 1781, 1782, 1783, 0, 1784 };\
ht[72057596453847808] = { 1785, 0, 0, 1786, 1787 };\
ht[72057596453848576] = { 1788, 0, 0, 1789, 1790 };\
ht[1152921505949025024] = { 1791, 1792, 1793, 0, 1794 };\
ht[288230378567630852] = { 1795, 0, 0, 1796, 1797 };\
ht[288230378567630851] = { 1798, 0, 0, 1799, 1800 };\
ht[288230378567631876] = { 1801, 0, 0, 1802, 1803 };\
ht[288230378567630849] = { 1804, 0, 0, 1805, 1806 };\
ht[288230378567630848] = { 1807, 0, 0, 1808, 1809 };\
ht[72057596453848065] = { 1810, 0, 0, 1811, 1812 };\
ht[288230378567632386] = { 1813, 0, 0, 1814, 1815 };\
ht[9223372037794300674] = { 1816, 0, 0, 0, 1817 };\
ht[72057596453847298] = { 1818, 0, 0, 1819, 1820 };\
ht[288230378567632128] = { 1821, 0, 0, 1822, 1823 };\
ht[1152921505949024512] = { 1824, 1825, 1826, 0, 1827 };\
ht[18014399583223814] = { 1828, 1829, 1830, 0, 1831 };\
ht[144115190491774982] = { 1832, 0, 0, 1833, 1834 };\
ht[576460753645601026] = { 1835, 1836, 1837, 0, 1838 };\
ht[36028798092706048] = { 1839, 1840, 1841, 0, 1842 };\
ht[576460753645601024] = { 1843, 1844, 1845, 0, 1846 };\
ht[9223372037794299904] = { 1847, 0, 0, 0, 1848 };\
ht[2305843010555871236] = { 1849, 1850, 1851, 0, 1852 };\
ht[576460753645602310] = { 1853, 1854, 1855, 0, 1856 };\
ht[576460753645602308] = { 1857, 1858, 1859, 0, 1860 };\
ht[576460753645602307] = { 1861, 1862, 1863, 0, 1864 };\
ht[576460753645602306] = { 1865, 1866, 1867, 0, 1868 };\
ht[576460753645602305] = { 1869, 1870, 1871, 0, 1872 };\
ht[576460753645602304] = { 1873, 1874, 1875, 0, 1876 };\
ht[144115190491776258] = { 1877, 0, 0, 1878, 1879 }
