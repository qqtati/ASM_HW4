//
// Created by qqtati on 13.12.2022.
//

/*
 *Класс продавца
 */
class Seller {
 public:
  int id{};

  Seller() = default;

  // Раскрасить вывод Seller
  static string getColor(const string &line, int id) {
#if COLORS == 1
    if (answer == "2") {
      return line;
    }
    switch (id) {
      case 0:
        return ANSI_YELLOW + line + ANSI_RESET;
        case 1:
          return ANSI_GREEN + line + ANSI_RESET;
          case 2:
            return ANSI_CYAN + line + ANSI_RESET;
            case 3:
              return ANSI_PURPLE + line + ANSI_RESET;
              case 4:
                return ANSI_RED + line + ANSI_RESET;
                default:
                  return line;
    }
#else
return line;
#endif
  }
};