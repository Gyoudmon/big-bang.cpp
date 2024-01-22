#include "chromalet.hpp"

#include "../../datum/box.hpp"
#include "../../datum/flonum.hpp"

#include "../../graphics/image.hpp"
#include "../../physics/color/rgba.hpp"
#include "../../physics/color/names.hpp"

#include "../../physics/mathematics.hpp"

using namespace GYDM;

/*************************************************************************************************/
#define PseudoPrimaryColorCount sizeof(this->pseudo_primaries) / sizeof(RGBA)
static const float triangle_vertice_radius = 3.0F;

static const double red_wavelength = 700.0;
static const double green_wavelength = 546.1;
static const double blue_wavelength = 435.8;

static const double start_wavelength = 380.0;

static const double wavelength_xbars [] = {
    0.001435025, 0.0015721889999999999, 0.0017223550000000044, 0.0018990695000000056, 0.0021158785, 0.0023856924999999998,
    0.002713994, 0.0030967160000000002, 0.0035270325, 0.003998118, 0.0045026944999999995, 0.005046218499999984,
    0.0056543800000000005, 0.006359914500000022, 0.007195558499999999, 0.0082006865, 0.0093901265, 0.01072529,
    0.012145355, 0.013589505, 0.015007214999999999, 0.016425934999999958, 0.017964329999999955, 0.019764615,
    0.021969005, 0.02469868, 0.02799492, 0.031831700000000004, 0.03617458, 0.040989120000000004,
    0.0462528, 0.0520091, 0.0583707, 0.06546539999999999, 0.073421, 0.082294055,
    0.092067415, 0.10279151, 0.11458675, 0.1275736, 0.1418691, 0.15737695000000002,
    0.1736894, 0.19029705000000002, 0.2066905, 0.2224784, 0.23753325, 0.2518285,
    0.2652926, 0.27785395, 0.2894219, 0.29992015, 0.3093419, 0.31771635,
    0.3250727, 0.33142565, 0.33678070000000004, 0.3411657, 0.34462545, 0.3472048,
    0.34893995, 0.34987365000000004, 0.3500802, 0.34965, 0.34867349999999997, 0.34721665,
    0.34531785000000004, 0.3430356, 0.34045145, 0.33764705, 0.33469885, 0.3316194,
    0.3283384, 0.32476125, 0.3207934, 0.31636255, 0.31145455, 0.3060872,
    0.30027415, 0.29402895, 0.28738505000000003, 0.28034575, 0.2728196, 0.26467025,
    0.25576135, 0.24597375, 0.23534935, 0.2241292, 0.21260935, 0.20108575,
    0.1897868, 0.17877045, 0.16800769999999998, 0.1574857, 0.14719165, 0.13713930000000002,
    0.1273741, 0.11792240000000001, 0.10878655000000001, 0.09996895, 0.091469775, 0.083303795,
    0.0755129, 0.06814937500000001, 0.0612655, 0.054906060000000007, 0.049071815000000005, 0.0437162,
    0.038781855000000004, 0.034211414999999995, 0.0299636, 0.0260308, 0.0224157, 0.0191137,
    0.0161202, 0.013430895, 0.011040875, 0.0089436, 0.007131793000000001, 0.005598173,
    0.0043385865, 0.0033612464999999998, 0.0026851, 0.0023305864999999997, 0.0023181465, 0.00266276,
    0.00338104, 0.0045057, 0.00607846, 0.008141039999999999, 0.010724745, 0.013842685,
    0.0175067, 0.021735145, 0.026546385, 0.031957425, 0.037967505, 0.0445526,
    0.051681825, 0.05932430500000001, 0.067452505, 0.07604862500000001, 0.0851011, 0.09459820499999999,
    0.104528225, 0.1148837, 0.12564095, 0.1367412, 0.14811105, 0.15967710000000002,
    0.17137855000000002, 0.18319855000000002, 0.19515470000000001, 0.2072676, 0.21955785, 0.2320354,
    0.24469385, 0.25752949999999997, 0.27054695, 0.28375085, 0.2971456, 0.31073189999999995,
    0.3245055, 0.3384606, 0.3525914, 0.36689195, 0.38136175, 0.396009,
    0.41084495, 0.4258807, 0.44112260000000003, 0.45656565000000005, 0.4722, 0.48801764999999997,
    0.5040107, 0.520173, 0.53649375, 0.5529505, 0.5695154499999999, 0.58616075,
    0.60286045, 0.61959835, 0.636368, 0.6531649500000001, 0.6699848500000001, 0.6868196,
    0.7036488999999999, 0.7204435, 0.7371736, 0.7538094, 0.7703215999999999, 0.7866844,
    0.802876, 0.8188756, 0.8346624, 0.85021625, 0.86550705, 0.8804879999999999,
    0.89510625, 0.9093090500000001, 0.92304975, 0.9362989500000001, 0.949038, 0.96124775,
    0.97290895, 0.9839928, 0.9944672, 1.004319, 1.0135478, 1.0221532,
    1.03014135, 1.03748435, 1.044087, 1.0498273500000002, 1.0545833500000001, 1.0582471999999998,
    1.0607967999999999, 1.062303, 1.0628582, 1.0625548, 1.0614676, 1.0595894000000001,
    1.0568339999999998, 1.0531006, 1.0482884000000001, 1.04231845, 1.03519885, 1.0270135,
    1.01785695, 1.0078238499999999, 0.99698375, 0.98534945, 0.9729114999999999, 0.95966975,
    0.94562395, 0.930797, 0.915219, 0.898873, 0.881711, 0.86368495,
    0.84476695, 0.825015, 0.8045659999999999, 0.78357, 0.762177, 0.7404918,
    0.7185862000000001, 0.6965955, 0.6747063, 0.6531051999999999, 0.63195745, 0.61131435,
    0.5911095, 0.57125145, 0.5516488500000001, 0.5322497500000001, 0.51307295, 0.49414499999999995,
    0.47546875, 0.45704695, 0.43888065, 0.42097965000000004, 0.40337100000000004, 0.38608865000000003,
    0.36916665000000004, 0.35262815000000003, 0.33648655, 0.320768, 0.30551015000000004, 0.29075055,
    0.27652239999999995, 0.2628315999999993, 0.24965399999999927, 0.2369584, 0.2247136, 0.21289855000000002,
    0.20151015, 0.19054700000000063, 0.1800015500000006, 0.16986615, 0.16013335, 0.15079835,
    0.14185999999999999, 0.13331835, 0.12517335000000002, 0.11741984999999999, 0.11005235, 0.10307772000000001,
    0.096510525, 0.090365305, 0.08465048, 0.07935262, 0.0744407, 0.06988188000000001,
    0.06564332, 0.061703425000000006, 0.058044504999999795, 0.05462659999999981, 0.051394825, 0.048294304999999996,
    0.045277025, 0.042329704999999995, 0.03947400000000016, 0.03673862500000015, 0.034152305, 0.031732095,
    0.029472375000000002, 0.0273627, 0.025398795000000002, 0.023576375, 0.021892145000000002, 0.020342085,
    0.0189186, 0.017612245, 0.016413585, 0.01531532, 0.014310980000000001, 0.013389999999999999,
    0.01253894, 0.01174418, 0.010994255, 0.010284097999999959, 0.009613633999999963, 0.008983637999999999,
    0.008394885, 0.007846651999999999, 0.0073355669999999994, 0.006858029500000026, 0.006411360500000025, 0.005992877000000001,
    0.005600086000000001, 0.0052312045, 0.0048850475, 0.0045605095000000005, 0.004256482000000001, 0.003971685,
    0.0037048305, 0.0034550450000000003, 0.0032217085, 0.003004201, 0.0028018375000000003, 0.002613684,
    0.0024385939999999997, 0.002275392, 0.002122903, 0.001980075, 0.0018461989999999926, 0.001720773999999993,
    0.0016032845000000001, 0.001493215, 0.0013900065, 0.0012931585000000002, 0.0012023730000000048, 0.0011174505000000046,
    0.0010381894999999999, 0.0009643424999999999, 0.0008955845, 0.0008315915, 0.0007720730000000001, 0.0007167375,
    0.0006652975, 0.000617509, 0.0005731835000000001, 0.000532147, 0.000494225, 0.0004592375,
    0.00042698300000000003, 0.00039724650000000003, 0.000369815, 0.000344475, 0.00032103000000000003, 0.0002993229999999988,
    0.00027921299999999886, 0.0002605535, 0.00024319699999999998, 0.0002269985, 0.0002118485, 0.0001976835000000008,
    0.0001844530000000007, 0.000172108, 0.0001605875, 0.000149823, 0.000139766, 0.000130381,
    0.0001216325, 0.00011348249999999999, 0.00010588850000000001, 9.880975000000001e-5, 9.22092e-5, 8.60496e-5,
    8.029399999999999e-5, 7.491285e-5, 6.98854e-5, 6.51931e-5, 6.081745e-5, 5.67384e-5,
    5.293474999999979e-5, 4.938784999999981e-5, 4.60811e-5, 4.29978e-5
};

static const double wavelength_ybars [] = {
    4.09132e-5, 4.4870500000000003e-5, 4.925210000000014e-5, 5.438300000000016e-5, 6.05882e-5, 6.81721e-5,
    7.72782e-5, 8.78602e-5, 9.98221e-5, 0.000113068, 0.000127492, 0.00014323799999999953,
    0.00016085, 0.00018101200000000063, 0.000204408, 0.00023195349999999997, 0.0002640735, 0.00029988,
    0.0003378935, 0.0003766335, 0.0004148575, 0.0004533694999999989, 0.0004954499999999987, 0.0005450475000000001,
    0.0006061095, 0.00068228, 0.0007750299999999999, 0.00088333, 0.00100552, 0.0011399399999999999,
    0.0012860455, 0.0014464215, 0.0016255599999999999, 0.0018278455, 0.0020576615000000003, 0.0023174,
    0.0026094, 0.0029408999999999998, 0.0033220999999999997, 0.0037632, 0.00427312, 0.00485278,
    0.005494300000000001, 0.00618772, 0.00692308, 0.0076932535, 0.0084976135, 0.009338200000000001,
    0.010216055000000002, 0.011132215, 0.012086585, 0.013077945, 0.0141062, 0.015172385,
    0.016277545, 0.01742368, 0.01861092, 0.0198342, 0.02108608, 0.02235912,
    0.023647305, 0.024952425, 0.026284405, 0.02765491, 0.029075625, 0.030555415,
    0.032097255, 0.0337024, 0.035373415000000005, 0.037112855, 0.038923335, 0.040807335,
    0.042767, 0.044804335, 0.046921335, 0.04912184, 0.05140836, 0.0537768,
    0.05621964, 0.058729359999999994, 0.061300985, 0.063939745, 0.0666598, 0.06947658500000001,
    0.07240554499999999, 0.075458, 0.0786412, 0.0819666, 0.08544979999999999, 0.0891064,
    0.09294877500000001, 0.096981695, 0.10120662, 0.105626, 0.1102423, 0.115066,
    0.1201032, 0.1253336, 0.1307228, 0.1362364, 0.1418482, 0.14757285,
    0.15346559999999998, 0.1595898, 0.16600884999999999, 0.17277155, 0.17990060000000002, 0.18741580000000002,
    0.19534575, 0.203719, 0.21256995, 0.2219272, 0.2317958, 0.24216915,
    0.2530406, 0.26439245, 0.2762394, 0.2886722, 0.30181425, 0.315789,
    0.33070105, 0.34654395, 0.36319219999999997, 0.38049305, 0.39829375, 0.41646495,
    0.43496975000000004, 0.45385200000000003, 0.47316695, 0.49296975, 0.51328465, 0.53404065,
    0.5551010000000001, 0.5763276500000001, 0.59758265, 0.6187727999999999, 0.6398261999999999, 0.6605909999999999,
    0.6808588, 0.7004212, 0.7190926, 0.7368243999999999, 0.7537165, 0.7699031000000001,
    0.7855184, 0.8006552, 0.8153033000000001, 0.8294015, 0.8428992, 0.8557458,
    0.8679053999999999, 0.8793866, 0.890228, 0.9004684000000001, 0.91014665, 0.9192924499999999,
    0.9279136, 0.9360075000000001, 0.9435739000000001, 0.9506125999999999, 0.9571280499999999, 0.96313175,
    0.968634, 0.97364155, 0.97816125, 0.9821962, 0.9857553, 0.9888655,
    0.9915622, 0.99388085, 0.9958304499999999, 0.99740455, 0.9986051499999999, 0.9994301,
    0.9998741, 0.99992835, 0.99958065, 0.9988150499999999, 0.9976121, 0.9959493500000001,
    0.99380025, 0.99117155, 0.9880935, 0.9845842499999999, 0.9806620500000001, 0.9763418500000001,
    0.97162745, 0.966514, 0.96099585, 0.95506745, 0.9487251999999999, 0.9419748,
    0.934831, 0.9273102, 0.9194287999999999, 0.9112032, 0.9026418, 0.893741,
    0.8844932000000001, 0.8748908, 0.86493065, 0.85462665, 0.844007, 0.8331016499999999,
    0.8219406499999999, 0.81054735, 0.79893835, 0.787137, 0.77517335, 0.76307735,
    0.7508770499999999, 0.7385882500000001, 0.726213, 0.71375005, 0.70119825, 0.6885596,
    0.6758454, 0.663073, 0.6502596, 0.6374224, 0.62457775, 0.61173495,
    0.598895, 0.58605675, 0.57321895, 0.56038055, 0.54754915, 0.534745,
    0.52199255, 0.5093161500000001, 0.4967344, 0.4842496, 0.471854, 0.4595404,
    0.44730159999999997, 0.43513999999999997, 0.42305800000000005, 0.411034, 0.399032, 0.387016,
    0.3749592, 0.3628728, 0.350802, 0.3387972, 0.3269088, 0.31516905,
    0.30359425, 0.292222, 0.28110904999999997, 0.27031225000000003, 0.25988160000000005, 0.2498264,
    0.240112, 0.2306936, 0.2215264, 0.21258080000000001, 0.20385520000000001, 0.195352,
    0.1870648, 0.1789872, 0.17111175, 0.16343495, 0.155962, 0.14870175000000002,
    0.14166295, 0.13485015, 0.12826254999999998, 0.121902, 0.11577414999999999, 0.10988455,
    0.1042381, 0.0988324199999997, 0.0936557999999997, 0.088693905, 0.083932425, 0.07936032000000001,
    0.07497308, 0.07076780000000024, 0.06673992000000023, 0.06288488, 0.059198105, 0.055675625000000006,
    0.0523144, 0.049111705, 0.046064825000000004, 0.043169360000000004, 0.04042184, 0.0378244,
    0.035382159999999996, 0.03310024, 0.030981305, 0.029019625, 0.027202999999999998, 0.025518705,
    0.023954024999999997, 0.022500384999999998, 0.021150944999999928, 0.019891099999999932, 0.018700885, 0.017560344999999998,
    0.016451895, 0.015370485, 0.014323930000000058, 0.013322730000000057, 0.01237739, 0.011494154999999999,
    0.010670849999999999, 0.0099033505, 0.009189734, 0.008528078500000001, 0.007916890499999999, 0.0073546025,
    0.006838449999999999, 0.0063649805, 0.0059307425, 0.0055330295, 0.0051694275000000005, 0.0048361,
    0.0045282395, 0.004241037499999999, 0.0039702265, 0.003713775999999986, 0.0034716589999999863, 0.0032441559999999998,
    0.0030315465, 0.0028335695, 0.0026490075, 0.0024765600000000096, 0.0023152595000000085, 0.0021641374999999997,
    0.0020222935000000003, 0.0018890835, 0.00176408, 0.0016468835, 0.0015370935, 0.001434248,
    0.0013378819999999999, 0.00124768, 0.0011634179999999998, 0.001084872, 0.001011795, 0.0009438495,
    0.000880621, 0.0008216855, 0.0007666190000000001, 0.0007150415, 0.0006666964999999974, 0.0006214029999999976,
    0.0005789755, 0.0005392274999999999, 0.0005019569999999999, 0.0004669835, 0.0004341990000000018, 0.0004035315000000017,
    0.000374909, 0.000348242, 0.000323412, 0.00030030300000000003, 0.00027881, 0.000258827,
    0.00024025099999999997, 0.000222994, 0.000206987, 0.000192168, 0.000178474, 0.00016583899999999998,
    0.00015419099999999997, 0.00014345299999999998, 0.0001335475, 0.00012439650000000001, 0.00011593000000000001, 0.00010809099999999956,
    0.00010082879999999961, 9.409075e-5, 8.782295e-5, 8.197335000000001e-5, 7.650235e-5, 7.138700000000028e-5,
    6.660935000000027e-5, 6.215135e-5, 5.7990950000000004e-5, 5.410375e-5, 5.0472e-5, 4.708295e-5,
    4.3923750000000004e-5, 4.09805e-5, 3.823805e-5, 3.56819e-5, 3.3298449999999996e-5, 3.10741e-5,
    2.899565e-5, 2.7052450000000002e-5, 2.5236900000000002e-5, 2.35424e-5, 2.1962299999999998e-5, 2.04893e-5,
    1.9115749999999926e-5, 1.7834899999999933e-5, 1.664075e-5, 1.55273e-5
};

static const double wavelength_zbars [] = {
    0.0067666085, 0.007414352, 0.008123320000000022, 0.008957848000000025, 0.009982267, 0.011257895,
    0.012810835, 0.01462196, 0.0166591, 0.01889008, 0.021280685, 0.023857119999999926,
    0.026741300000000003, 0.030088380000000105, 0.034053520000000004, 0.038823855000000004, 0.044470715, 0.050811800000000004,
    0.057558955, 0.06442402, 0.071168165, 0.0779239399999998, 0.08525759999999978, 0.09384706000000001,
    0.10437024, 0.11740665, 0.1331575, 0.1515026, 0.17228015, 0.1953284,
    0.22054605, 0.24815175, 0.27869299999999997, 0.31278655, 0.35104925, 0.39375455000000004,
    0.44083665, 0.4925795, 0.54961255, 0.61256515, 0.6820419, 0.75759855,
    0.8372796, 0.91864245, 0.99924455, 1.0772087, 1.1519322, 1.2233102,
    1.29102645, 1.3547647999999999, 1.4141176, 1.46871935, 1.5184969, 1.5635355,
    1.60392035, 1.6396823999999999, 1.67085035, 1.6975852, 1.7201282999999998, 1.73872105,
    1.7535523, 1.7648339499999999, 1.7729434999999998, 1.77834855, 1.7815167, 1.7827841,
    1.782334, 1.780449, 1.77753265, 1.7739885499999999, 1.77018445, 1.7661489499999998,
    1.7614914, 1.75570505, 1.74828315, 1.7388297499999998, 1.7272088, 1.7133975000000001,
    1.69733705, 1.6789686000000001, 1.65836435, 1.6354707, 1.6097175, 1.5802751499999999,
    1.546314, 1.5071056999999999, 1.46281645, 1.4147007, 1.36430805, 1.3131881,
    1.26253115, 1.2126233, 1.1632927, 1.11445455, 1.066024, 1.0180488,
    0.97077245, 0.9244002, 0.8790362, 0.8347846999999999, 0.7917337, 0.7499810499999999,
    0.7096792000000001, 0.67100925, 0.63415245, 0.5992643, 0.5663724, 0.5353768999999999,
    0.50615245, 0.47857364999999996, 0.4525523, 0.4280541, 0.40503290000000003, 0.38341405,
    0.36312295, 0.34407889999999997, 0.32620495, 0.30944479999999996, 0.29375304999999996, 0.2790843,
    0.26540855, 0.25265045, 0.2406278, 0.22911255, 0.21787665, 0.2067346,
    0.1956444, 0.18467250000000002, 0.17389310000000002, 0.1633804, 0.15316915, 0.14325705,
    0.133685, 0.12453465, 0.11588755, 0.10780239999999999, 0.10028614, 0.0933251,
    0.08691401500000001, 0.08104765, 0.07572949000000001, 0.07094357500000001, 0.066623, 0.062678095,
    0.05901918, 0.05557718, 0.052325495, 0.0492497, 0.046325675, 0.043529295,
    0.040833640000000004, 0.03822146, 0.035697, 0.033273540000000004, 0.03096436, 0.028775904999999997,
    0.026703125, 0.0247408, 0.022888205000000002, 0.021144625, 0.019509025, 0.017979204999999998,
    0.016551999999999997, 0.015224124999999998, 0.013992305, 0.012853615, 0.011804555000000001, 0.0108399,
    0.009953613, 0.0091396525, 0.0083925995, 0.0077084, 0.0070834999999999995, 0.0065141,
    0.0059963995, 0.0055267995, 0.005101700000000001, 0.004717000000000001, 0.0043683, 0.0040512,
    0.0037616, 0.0034969, 0.003256, 0.0030381, 0.0028423995, 0.0026675995000000003,
    0.0025119, 0.0023740000000000002, 0.0022531, 0.0021484, 0.0020588665, 0.0019829665,
    0.0019190000000000001, 0.0018653665, 0.0018204665, 0.0017831334999999999, 0.0017520335, 0.0017245,
    0.0016971335, 0.001666534, 0.0016300669999999998, 0.0015872665, 0.001539, 0.0014860665,
    0.0014292665, 0.0013683335, 0.0013033335, 0.0012374999999999999, 0.0011758335, 0.0011233335,
    0.0010844000000000001, 0.0010590999999999999, 0.0010425, 0.0010284, 0.0010106, 0.00098432,
    0.00094928, 0.0009084, 0.00086472, 0.00082128, 0.0007804800000000001, 0.00074232,
    0.0007048, 0.00066568, 0.00062272, 0.0005739335, 0.0005197335, 0.00046350000000000004,
    0.0004094335, 0.0003617335, 0.0003236265, 0.0002952065, 0.0002743, 0.0002586265,
    0.0002459065, 0.0002347735, 0.0002250935, 0.0002163, 0.0002070735, 0.0001960935,
    0.0001821065, 0.00016492649999999998, 0.0001458, 0.0001264065, 0.0001084265, 9.306665e-5,
    8.036665e-5, 6.979999999999999e-5, 6.096665e-5, 5.346665e-5, 4.708e-5, 4.182e-5,
    3.76e-5, 3.418e-5, 3.132e-5, 2.882665e-5, 2.660665e-5, 2.46e-5,
    2.272665e-5, 2.090665e-5, 1.906665e-5, 1.7166650000000003e-5, 1.52e-5, 1.316665e-5,
    1.106665e-5, 8.866665e-6, 6.566665e-6, 4.2999999999999995e-6, 2.266665e-6, 6.66665e-7,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0
};

/*************************************************************************************************/
GYDM::Chromalet::Chromalet(float width, float height, CIE_Standard std, double Y)
        : width(width), height(height), standard(std), pseudo_primary_triangle_color(GRAY), luminance(Y) {
    if (this->height == 0.0) {
        this->height = flabs(this->width);
    }

    this->recalculate_primary_colors();
}

Box GYDM::Chromalet::get_bounding_box() {
    return { flabs(this->width), flabs(this->height) };
}

void GYDM::Chromalet::on_resize(float w, float h, float width, float height) {
    ICanvaslet::on_resize(w, h, width, height);

    this->width = w;
    this->height = h;
}

void GYDM::Chromalet::draw_before_canvas(GYDM::dc_t* dc, float flx, float fly, float flwidth, float flheight) { 
    dc->draw_grid(10, 10, (flwidth - 2.0F) / 10.0F, (flheight - 2.0F)/ 10.0F, DIMGRAY, flx, fly);
    SDL_RenderDrawLineF(dc->self(), flx, fly, flx + flwidth, fly + flheight);
}

void GYDM::Chromalet::draw_on_canvas(GYDM::dc_t* dc, float flwidth, float flheight) {
    double imaginary_width = abs(this->width);
    double imaginary_height = abs(this->height);
 
    // this->draw_color_map(dc, imaginary_width, imaginary_height);
    this->draw_spectral_locus(dc, imaginary_width, imaginary_height);
    this->draw_chromaticity(dc, imaginary_width, imaginary_height);
}

void GYDM::Chromalet::draw_after_canvas(GYDM::dc_t* dc, float flx, float fly, float flwidth, float flheight) {
    if (this->pseudo_primary_triangle_color.is_opacity()) {
        this->draw_color_triangle(dc, flx, fly);
    }
}

bool GYDM::Chromalet::is_colliding(const Dot& local_pt) {
    return this->is_point_inside_the_spectrum(double(local_pt.x), double(local_pt.y));
}

/*************************************************************************************************/
RGBA GYDM::Chromalet::get_color_at(double mx, double my, bool after_special) {
    double imaginary_width = abs(this->width);
    double imaginary_height = abs(this->height);
    RGBA c = 0U;

    if (after_special) {
        double pcxs[PseudoPrimaryColorCount + 1];
        double pcys[PseudoPrimaryColorCount + 1];

        for (size_t idx = 0; idx < PseudoPrimaryColorCount; idx ++) {
            this->feed_color_location(this->pseudo_primaries[idx], &pcxs[idx], &pcys[idx]);

            if (point_distance(pcxs[idx], pcys[idx], mx, my) <= triangle_vertice_radius) {
                c = this->get_color_at(pcxs[idx], pcys[idx]);
                break;
            }
        }

        if (c.is_black()) { // check midpoints
            pcxs[PseudoPrimaryColorCount] = pcxs[0];
            pcys[PseudoPrimaryColorCount] = pcys[0];

            for (size_t idx = 0; idx < PseudoPrimaryColorCount; idx ++) {
                double pcmx, pcmy;

                line_point(pcxs[idx], pcys[idx], pcxs[idx + 1], pcys[idx + 1], 0.5, &pcmx, &pcmy);
                if (point_distance(pcmx, pcmy, mx, my) <= triangle_vertice_radius) {
                    c = this->get_color_at(pcmx, pcmy, false);
                    break;
                }
            }
        }

        if (c.is_black()) { // check the white point
            double wx, wy;

            this->feed_color_location(0xFFFFFFU, &wx, &wy);
            if (point_distance(wx, wy, mx, my) <= triangle_vertice_radius) {
                c = 0xFFFFFFU;
            }
        }
    }

    if (c.is_black()) {
        double x = mx / imaginary_width;
        double y = 1.0 - my / imaginary_height;
        double X, Y, Z, R, G, B;
        
        if (x + y <= 1.0) {
            CIE_xyY_to_XYZ(x, y, &X, &Y, &Z, this->luminance);
            CIE_XYZ_to_RGB(this->standard, X, Y, Z, &R, &G, &B, true);

            if ((R >= 0.0) && (G >= 0.0) && (B >= 0.0)) {
                c = RGBA(R, G, B);
            } else if (this->is_point_inside_the_spectrum(mx, my)) {
                c = RGBA(R, G, B);
            }
        }
    }

    return c;
}

void GYDM::Chromalet::feed_pseudo_primary_color_location(size_t idx, float* x, float* y, float* fx, float* fy) {
    idx %= PseudoPrimaryColorCount;
    this->feed_color_location(this->pseudo_primaries[idx], x, y, fx, fy);
}

void GYDM::Chromalet::feed_pseudo_primary_color_location(size_t idx, double* x, double* y, double* fx, double* fy) {
    idx %= PseudoPrimaryColorCount;
    this->feed_color_location(this->pseudo_primaries[idx], x, y, fx, fy);
}

void GYDM::Chromalet::feed_color_location(const RGBA& c, float* x, float *y, float* fx, float* fy) {
    double flx, fly, co_x, co_y;

    this->feed_color_location(c, &flx, &fly, &co_x, &co_y);
    SET_VALUES(x, float(flx), y, float(fly));
    SET_VALUES(fx, float(co_x), fy, float(co_y));
}

void GYDM::Chromalet::feed_color_location(const RGBA& c, double* x, double *y, double* fx, double* fy) {
    if (!c.is_black()) {
        double X, Y, Z, co_x, co_y;
        double imaginary_width = abs(this->width);
        double imaginary_height = abs(this->height);
    
        CIE_RGB_to_XYZ(this->standard, c.red(), c.green(), c.blue(), &X, &Y, &Z);
        CIE_XYZ_to_xyY(X, Y, Z, &co_x, &co_y);   
        SET_VALUES(fx, co_x, fy, co_y);

        this->fix_render_location(&co_x, &co_y);
        SET_VALUES(x, co_x * imaginary_width, y, co_y * imaginary_height);
    } else {
        SET_VALUES(x, 0.0, y, 0.0);
        SET_VALUES(fx, 0.0, fy, 0.0);
    }
}

void GYDM::Chromalet::recalculate_primary_colors(int idx) {
    if (idx < 0) {
        for (int i = 0; i < PseudoPrimaryColorCount; i++) {
            this->recalculate_primary_colors(i);
        }
    } else {
        double R, G, B;
        int bar_idx = 0;
    
        idx %= PseudoPrimaryColorCount;

        switch (idx) {
        case 0: bar_idx = fl2fxi(red_wavelength - start_wavelength); break;
        case 1: bar_idx = fl2fxi(green_wavelength - start_wavelength); break;
        case 2: bar_idx = fl2fxi(blue_wavelength - start_wavelength); break;
        }
    
        CIE_XYZ_to_RGB(this->standard, wavelength_xbars[bar_idx], wavelength_ybars[bar_idx], wavelength_zbars[bar_idx], &R, &G, &B, true);
        this->pseudo_primaries[idx] = RGBA(R, G, B);
    }
}

/*************************************************************************************************/
void GYDM::Chromalet::draw_color_triangle(GYDM::dc_t* dc, double dx, double dy) {
    static const size_t vcount = PseudoPrimaryColorCount + 1U;
    SDL_FPoint pts[vcount];
    double x, y;

    for (int idx = 0; idx < PseudoPrimaryColorCount; idx ++) {
        this->feed_color_location(this->pseudo_primaries[idx], &x, &y);

        pts[idx].x = float(x + dx);
        pts[idx].y = float(y + dy);
    }

    pts[vcount - 1] = pts[0];
    dc->draw_lines(pts, vcount, this->pseudo_primary_triangle_color);

    for (int idx = 0; idx < PseudoPrimaryColorCount; idx ++) {
        if ((pts[idx].x != pts[idx + 1].x) || (pts[idx].y != pts[idx + 1].y)) {
            float mx, my;

            line_point(pts[idx].x, pts[idx].y, pts[idx + 1].x, pts[idx + 1].y, 0.5, &mx, &my);
            this->render_special_dot(dc, this->pseudo_primaries[idx], pts[idx].x, pts[idx].y);
            this->render_special_dot(dc, this->get_color_at(double(mx) - dx, double(my) - dy), mx, my);
        }
    }
}

void GYDM::Chromalet::draw_color_map(GYDM::dc_t* dc, double flwidth, double flheight, double dx, double dy) {
    double X, Y, Z, R, G, B, x, y;

    for (int px = 0; px < fl2fxi(flwidth); px ++) {
        for (int py = 0; py < fl2fxi(flheight); py ++) {
            x = double(px) / flwidth;
            y = double(py) / flheight;
                    
            if (x + y <= 1.0) {
                CIE_xyY_to_XYZ(x, y, &X, &Y, &Z, this->luminance);
                CIE_XYZ_to_RGB(this->standard, X, Y, Z, &R, &G, &B, true);

                if ((R >= 0.0) && (G >= 0.0) && (B >= 0.0)) {
                    this->render_dot(dc, x, y, flwidth, flheight, R, G, B, dx, dy);
                }
            }
        }
    }
}

void GYDM::Chromalet::draw_spectral_locus(GYDM::dc_t* dc, double flwidth, double flheight, double dx, double dy) {
    double R, G, B, xbar, ybar, zbar, x, y;
    
    for (int idx = 0; idx < sizeof(wavelength_xbars) / sizeof(double); idx ++) {
        xbar = wavelength_xbars[idx];
        ybar = wavelength_ybars[idx];
        zbar = wavelength_zbars[idx];

        CIE_XYZ_to_xyY(xbar, ybar, zbar, &x, &y);
        CIE_XYZ_to_RGB(this->standard, xbar, ybar, zbar, &R, &G, &B, false);
        this->render_dot(dc, x, y, flwidth, flheight, R, G, B, dx, dy);
    }
}

void GYDM::Chromalet::draw_chromaticity(GYDM::dc_t* dc, double flwidth, double flheight, double dx, double dy) {
    int slt_idx = this->scanline_idx0;
    int slb_idx = this->scanline_idx0;
    double X, Y, Z, R, G, B;
    double fx, fy, ty, by;
    
    if (this->locus_xs == nullptr) {
        this->make_locus_polygon(flwidth, flheight);
        slt_idx = slb_idx = this->scanline_idx0;
    }

    for (double x = flround(this->scanline_start); x <= this->scanline_end; x += 1.0) {
        fx = x / flwidth;
        this->spectrum_intersection_vpoints(x, flheight, slt_idx, slb_idx, &ty, &by);

        for (double y = flfloor(ty); y <= flceiling(by); y += 1.0) {
            fy = 1.0 - y / flheight;        
            CIE_xyY_to_XYZ(fx, fy, &X, &Y, &Z, this->luminance);
            CIE_XYZ_to_RGB(this->standard, X, Y, Z, &R, &G, &B, true);
            this->render_dot(dc, fx, fy, flwidth, flheight, R, G, B, dx, dy);
        }
    };
}

/*************************************************************************************************/
void GYDM::Chromalet::fix_render_location(double* x, double* y) {
    if (this->width < 0.0F) { (*x) = 1.0 - (*x); }
    if (this->height > 0.0F) { (*y) = 1.0 - (*y); }
}

void GYDM::Chromalet::render_special_dot(GYDM::dc_t* dc, const RGBA& c, float x, float y) {
    dc->fill_circle(x, y, triangle_vertice_radius, c);
    dc->draw_circle(x, y, triangle_vertice_radius, GHOSTWHITE);
    dc->draw_circle(x, y, triangle_vertice_radius + 1.0F, GHOSTWHITE);
}

void GYDM::Chromalet::render_dot(GYDM::dc_t* dc, double x, double y, double width, double height, double R, double G, double B, double dx, double dy, double A) {
    this->fix_render_location(&x, &y);
    dc->draw_point(float(x * width + dx), float(y * height + dy), RGBA(R, G, B, A));
}

void GYDM::Chromalet::make_locus_polygon(double flwidth, double flheight) {
    double xbar, ybar, zbar, x, y;

    this->locus_count = sizeof(wavelength_xbars) / sizeof(double) + 2;
    this->scanline_start = infinity;
    this->scanline_end = -infinity;

    this->locus_xs = new double[this->locus_count];
    this->locus_ys = new double[this->locus_count];

    for (int idx = 1; idx < this->locus_count - 1; idx ++) {
        xbar = wavelength_xbars[idx - 1];
        ybar = wavelength_ybars[idx - 1];
        zbar = wavelength_zbars[idx - 1];

        CIE_XYZ_to_xyY(xbar, ybar, zbar, &x, &y);

        this->fix_render_location(&x, &y);
        this->locus_xs[idx] = x * flwidth;
        this->locus_ys[idx] = y * flheight;

        if (this->locus_xs[idx] < this->scanline_start) {
            this->scanline_start = this->locus_xs[idx];
            this->scanline_idx0 = idx;
        }

        if (this->locus_xs[idx] > this->scanline_end) {
            this->scanline_end = this->locus_xs[idx];
        }
    }

    // make the polyline a polygon
    this->locus_xs[0] = this->locus_xs[this->locus_count - 2];
    this->locus_ys[0] = this->locus_ys[this->locus_count - 2];
    this->locus_xs[this->locus_count - 1] = this->locus_xs[1];
    this->locus_ys[this->locus_count - 1] = this->locus_ys[1];
}

bool GYDM::Chromalet::is_point_inside_the_spectrum(double mx, double my) {
    double imaginary_width = abs(this->width);
    double imaginary_height = abs(this->height);
    bool inside = false;
    
    if (this->locus_xs == nullptr) {
        this->make_locus_polygon(imaginary_width, imaginary_height);
    }

    if ((mx >= this->scanline_start) && (mx <= this->scanline_end)) {
        int slt_idx = this->scanline_idx0;
        int slb_idx = this->scanline_idx0;
        double ty, by;

        this->spectrum_intersection_vpoints(mx, imaginary_height, slt_idx, slb_idx, &ty, &by);
        inside = (flfloor(ty) <= my) && (my <= flceiling(by));
    }

    return inside;
}

void GYDM::Chromalet::spectrum_intersection_vpoints(double mx, double flheight, int& slt_idx, int& slb_idx, double* ty, double* by) {
    double t;

    while (true) {
        lines_intersect(this->locus_xs[slt_idx], this->locus_ys[slt_idx],
                this->locus_xs[slt_idx + 1], this->locus_ys[slt_idx + 1],
                mx, 0.0, mx, flheight, flnull, ty, &t);

        if (t <= 1.0) break;
        slt_idx ++;
    }

    while (true) {
        lines_intersect(this->locus_xs[slb_idx], this->locus_ys[slb_idx],
                this->locus_xs[slb_idx - 1], this->locus_ys[slb_idx - 1],
                mx, 0.0, mx, flheight, flnull, by, &t);

        if (t <= 1.0) break;
        slb_idx --;
    }
}

/*************************************************************************************************/
void GYDM::Chromalet::on_canvas_invalidated() {
    this->invalidate_locus();
}

void GYDM::Chromalet::invalidate_locus() {
    if (this->locus_xs != nullptr) {
        delete [] this->locus_xs;
        delete [] this->locus_ys;

        this->locus_xs = nullptr;
        this->locus_ys = nullptr;
    }
}

void GYDM::Chromalet::set_luminance(double Y) {
    if (this->luminance != Y) {
        this->luminance = Y;
        this->dirty_canvas();
    }
}

void GYDM::Chromalet::set_standard(CIE_Standard std) {
    if (this->standard != std) {
        this->standard = std;
        this->dirty_canvas();
    }    
}

void GYDM::Chromalet::set_pseudo_primary_color(const RGBA& color, size_t idx) {
    idx %= PseudoPrimaryColorCount;

    if (this->pseudo_primaries[idx] != color) {
        if (color == 0) {
            this->recalculate_primary_colors(int(idx));
        } else {
            this->pseudo_primaries[idx] = color;
        }

        this->notify_updated();
    }
}

void GYDM::Chromalet::set_pseudo_primary_triangle_color(const RGBA& color) {
    if (this->pseudo_primary_triangle_color != color) {
        this->pseudo_primary_triangle_color = color;
        this->notify_updated();
    }
}

void GYDM::Chromalet::set_pseudo_primary_triangle_alpha(double alpha) {
    if (this->pseudo_primary_triangle_color.alpha() != alpha) {
        this->pseudo_primary_triangle_color = RGBA(this->pseudo_primary_triangle_color, alpha);
        this->notify_updated();
    }
}
