#include <argparse.h>

void get_opts(int argc,
              char **argv,
              struct options_t *opts)
{
    if (argc == 1)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "\t--in or -i <file_path>" << std::endl;
        std::cout << "\t--out or -o <file_path>" << std::endl;
        std::cout << "\t--n_threads or -n <num_threads>" << std::endl;
        std::cout << "\t--loops or -l <num_loops>" << std::endl;
        std::cout << "\t[Optional] --spin or -s" << std::endl;
        exit(0);
    }

    opts->spin = false;

    struct option l_opts[] = {
        {"in", required_argument, NULL, 'i'},
        {"out", required_argument, NULL, 'o'},
        {"n_threads", required_argument, NULL, 'n'},
        {"loops", required_argument, NULL, 'l'},
        {"spin", no_argument, NULL, 's'}
    };

    int ind, c;
    while ((c = getopt_long(argc, argv, "i:o:n:p:l:", l_opts, &ind)) != -1)
    {
        switch (c)
        {
        case 0:
            break;
        case 'i':
            opts->in_file = (char *)optarg;
            break;
        case 'o':
            opts->out_file = (char *)optarg;
            break;
        case 'n':
            opts->n_threads = atoi((char *)optarg);
            break;
        case 's':
            opts->spin = true;
            break;
        case 'l':
            opts->n_loops = atoi((char *)optarg);
            break;
        case ':':
            std::cerr << argv[0] << ": option -" << (char)optopt << "requires an argument." << std::endl;
            exit(1);
        }
    }
}
