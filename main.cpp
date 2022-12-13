#include "main.h"
#include "Seller.cpp"
#include "Buyer.cpp"
// Вектор активных к обслуживанию покупателей (индекс = отдел)
vector<Buyer *> buyers;

// Стартовая функция потоков – продавцов (писателей)
void *SellerFunc(void *param) {
  string str_thread;
  Seller seller = *((Seller *) param);
  // Пока покупатели не кончились, работодатели пашут
  while (flag) {
    // Ждём пока не появится покупатель, чтобы не было null reference (не зависит от других потоков-продавцов)
    if (buyers[seller.id] == nullptr) {
      continue;
    }

    str_thread = Seller::getColor("\nПродавец: " + to_string(seller.id + 1) +
        " обслуживает покупателя с номером : " +
        to_string(buyers[seller.id]->id + 1) +
        " \t\tВремя : " + to_string((clock())),
        seller.id);
    cumulative += str_thread;
    cout << str_thread;
    // Обслуживает покупателя
    sleep(2);

    str_thread = Seller::getColor("\nПродавец с номером : " + to_string(seller.id + 1) +
        " закончил обслуживать покупателя с номером : " +
        to_string(buyers[seller.id]->id + 1) +
        " \t\tВремя : " + to_string((clock())),
        seller.id);
    cumulative += str_thread;
    cout << str_thread;

    buyers[seller.id] = nullptr;
  }
  str_thread = Seller::getColor("\nПродавец с номером : " + to_string(seller.id + 1) +
      " закончил работу. " +
      " \t\tВремя : " + to_string((clock())),
      seller.id);
  cumulative += str_thread;
  cout << str_thread;
  return nullptr;
}

// Стартовая функция потоков – покупателей (читателей)
void *BuyerFunc(void *param) {
  Buyer *all_buyers = ((Buyer *) param);
  string str_thread;
  // Покупатель засыпает на случайное время (не все же приходят к открытию)
  sleep(all_buyers->time_start);
  str_thread = "\nПокупатель с номером : " + to_string(all_buyers->id + 1) + " пришел в магазин." +
      " \t\t\tВремя : " + to_string(clock());
  cumulative += str_thread;
  cout << str_thread;
  while (!all_buyers->plan.empty()) {
    str_thread = Seller::getColor(
        "\nПокупатель с номером : " + to_string(all_buyers->id + 1) + " пришел к продавцу с номером: " +
        to_string(all_buyers->plan.front() + 1) + " \t Время : " + to_string(clock()),
        all_buyers->plan.front());
    cumulative += str_thread;
    cout << str_thread;

    // Покупатель идёт в отдел
    sleep(1);

    // Формирование очереди + защита операции чтения
    switch (all_buyers->plan.front()) {
      case 0:
        pthread_mutex_lock(&mutex_0);
        break;
        case 1:
          pthread_mutex_lock(&mutex_1);
          break;
          case 2:
            pthread_mutex_lock(&mutex_2);
            break;
            default:
              cout << Seller::getColor("\nПроблема в блокировке", 4);
    }

    // Занимаем место у кассы, тем самым подаём "сигнал" продавцу
    buyers[all_buyers->plan.front()] = all_buyers;

    // Покупатель разговаривает с продавцом
    sleep(1);

    // Заснуть пока продавец не закончит обрабатывать покупателя
    while (buyers[all_buyers->plan.front()] != nullptr) {
      usleep(200 * 1000);
    }

    // Пропускаем следующего из очереди + конец критической секции
    switch (all_buyers->plan.front()) {
      case 0:
        pthread_mutex_unlock(&mutex_0);
        break;
        case 1:
          pthread_mutex_unlock(&mutex_1);
          break;
          case 2:
            pthread_mutex_unlock(&mutex_2);
            break;
            default:
              cout << Seller::getColor("\nПроблема в разблокировке", 4);
    }
    // Отойдя от кассы, удаляем пункт из плана
    all_buyers->plan.pop();
  }
  str_thread = "\nПокупатель: " + to_string(all_buyers->id + 1) + " ушел из магазина" +
      " \t\t\t\tВремя : " + to_string(clock());
  cumulative += str_thread;
  cout << str_thread;
  return nullptr;
}

int startTheStore(int argc, char **argv) {
  // Сид генератора случайных чисел и просто числа
  int seed, num, val, max_count;

  // Работа с файлами
  string input, output;

  answer = "0";
  // argc == 3   =>   input.txt output.txt
  // argc == 4   =>   number_of_Buyers MAX_number_of_tasks seed
  if (argc == 3) {
    answer = "2";
    input = argv[1];
    output = argv[2];
  } else if (argc == 4) {
    answer = "3";
    num = stoi(argv[1]);
    max_count = stoi(argv[2]);
    seed = stoi(argv[3]);
  }

  if (answer == "0") {
    str = "Выберите входной формат данных :\n"
          " 1. Ввод с консоли.\n"
          " 2. Ввод из файла.\n"
          " 3. Ввод с помошью генерации случайных чисел.\n"
          "Ввод: ";
    cout << str;
    cin >> answer;
  }

  // Список продавцов
  std::vector<Seller> sellers(SECTIONS_NUM);
  std::vector<pthread_t> threads_sellers(sellers.size());

  // Список активных покупателей
  buyers = vector<Buyer *>(SECTIONS_NUM);

  // Список покупателей
  std::vector<Buyer> buyers;
  std::vector<pthread_t> threads_buyers;

  // Инициализация mutex
  pthread_mutex_init(&mutex_0, nullptr);
  pthread_mutex_init(&mutex_1, nullptr);
  pthread_mutex_init(&mutex_2, nullptr);

  if (answer == "1") {
    // Ручной ввод в консоль.
    cout << "Введите количество покупателей: ";
    cin >> num;
    buyers = vector<Buyer>(num);
    threads_buyers = vector<pthread_t>(buyers.size());
    // Создаём покупателей (дома готовят план покупок)
    for (int i = 0; i < buyers.size(); ++i) {
      buyers[i].id = i;
      // Создаём план для покупателя по полученным данным из консоли
      cout << "Покупатель с номером : " + to_string(i + 1);
      cout << "\nВведите количество заданий : ";
      cin >> num;
      if (num < 1) {
        continue;
      }
      queue<int> plan;
      str = "Введите " + to_string(num) + " номера отделов от (" +
          to_string(1) + " до " + to_string(SECTIONS_NUM) + "): ";
      cout << str;
      for (int j = 0; j < num; ++j) {
        cin >> val;
        plan.push(max(0, val - 1) % SECTIONS_NUM);
      }
      buyers[i].plan = plan;
    }
  } else if (answer == "2") {
    if (argc != 3) {
      // Ввод с помощью файла.
      cout << "Введите имя файла из которого будут браться данные : ";
      cin >> input;
      cout << "Введите имя файла в которой будет производиться запись: ";
      cin >> output;
    }
    // Поток файла ввода
    ifstream in(input);
    if (!in.is_open()) {
      cout << "Ошибка в открытии" << endl;
      return 1;
    }
    in >> num;
    buyers = vector<Buyer>(num);
    threads_buyers = vector<pthread_t>(buyers.size());
    for (int i = 0; i < buyers.size(); ++i) {
      buyers[i].id = i;
      // Количество деталей плана
      in >> num;
      queue<int> plan;
      for (int j = 0; j < num; ++j) {
        in >> val;
        plan.push(max(0, val - 1) % SECTIONS_NUM);
      }
      buyers[i].plan = plan;
    }
    in.close();
  } else {
    // Случайная генерация.
    if (argc != 4) {
      cout << "Введите количество покупателей : ";
      cin >> num;
      cout << "Введите максимальное количество задания для покупателя >0 : ";
      cin >> max_count;
      cout << "Введите сид для рандомной генерации: ";
      cin >> seed;
    }
    buyers = vector<Buyer>(num);
    threads_buyers = vector<pthread_t>(buyers.size());
    srand(seed);
    // Создаём покупателей выбирают план покупок
    for (int i = 0; i < buyers.size(); ++i) {
      buyers[i].id = i;
      // Создаём случайный план для покупателя
      num = 1 + rand() % max(1, max_count);
      queue<int> plan;
      for (int j = 0; j < num; ++j) {
        val = rand() % SECTIONS_NUM;
        plan.push(val);
      }
      buyers[i].plan = plan;
    }
  }

  str = Seller::getColor("\nМагазин открылся\n\n", 3);
  cumulative += str;
  cout << str;

  // Запуск 3 продавцов, по одному в каждом отделе
  for (int i = 0; i < sellers.size(); i++) {
    // Зададим id для продавца
    sellers[i].id = i;
    // Запускаем поток
    pthread_create(&threads_sellers[i], nullptr, SellerFunc, &sellers[i]);
    str = Seller::getColor(
        "\nПродавец с номером: " + to_string(sellers[i].id + 1) + " пришел на работу ", i);
    cumulative += str;
    cout << str;
  }

  // Информация о покупателях
  for (int i = 0; i < buyers.size(); ++i) {
    buyers[i].id = i;
    // Случайное время, когда покупатель придёт в магазин
    buyers[i].time_start = rand() % 14;
    str = "\nПокупатель с номером: " + to_string(buyers[i].id + 1) +
        " подготавливает план:" + buyers[i].getPlanToString();
    cumulative += str;
    cout << str;
  }
  cout << "\n";

  // Запускаем потоки покупателей
  for (int i = 0; i < buyers.size(); ++i) {
    pthread_create(&threads_buyers[i], nullptr, BuyerFunc, &buyers[i]);
  }

  // Тут параллельно происходит взаимодействие продавцов и покупателей
  // Ждём пока кончатся все покупатели
  for (_opaque_pthread_t *threads_buyer: threads_buyers) {
    pthread_join(threads_buyer, nullptr);
  }

  // Завершаем работу продавцов
  flag = false;

  cout << "\n";

  // Ждём пока закроются все отделы
  for (_opaque_pthread_t *threads_seller: threads_sellers) {
    pthread_join(threads_seller, nullptr);
  }

  str = Seller::getColor("\n\nМагазин закрылся", 3);
  cumulative += str;
  cout << str;

  if (answer == "2") {
    ofstream out(output);
    if (!out.is_open()) {
      cout << "Ошибка в открытии" << endl;
      return 0;
    }
    out << cumulative;
  }

  pthread_mutex_destroy(&mutex_0);
  pthread_mutex_destroy(&mutex_1);
  pthread_mutex_destroy(&mutex_2);

  return 0;
}

int main(int argc, char **argv) {
  try {
    int res = startTheStore(argc, argv);
    if (res == 1) {
      return 1;
    }
    cout << "\nВведите что-нибудь чтобы завершить программу:";
    cin >> str;
    return 0;
  } catch (const std::exception &exception) {
    cout << "\nОшибка, появилось исключение" << endl;
    return 1;
  }
}