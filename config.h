// absolute path. example
static const char path_to_gtasa[] = "/home/qwbash/.wine/drive_c/Program Files (x86)/Rockstar Games/GTA San Andreas";
static const char path_to_arizona[] = "/home/qwbash/.wine/drive_c/users/qwbash/AppData/Local/Programs/Arizona Games Launcher/bin/arizona/";

#define DEFAULT_MEM "4096"
#define DEFAULT_PORT "7777"

// for fix broken ru symbols (kryakoziabri)
// do export LC_ALL=ru_RU.UtF-8 before launch game
#define EXPORT_RU_LOCALE 1 // 1 - yes ; 0 - no

static const Server servers[] = {
    // ip             port            servername          nickname
    { "109.69.58.7", DEFAULT_PORT, "HackMySoftware", "cli_samp_client" },
    { "mirage.arizona-rp.com", DEFAULT_PORT, "Arizona Mirage", "Root"},
};
