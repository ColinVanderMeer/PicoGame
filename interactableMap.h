struct interactableObject {
    int x, y;
    hagl_bitmap_t* sprite;
    wchar_t message[];
};

struct interactableObject sign1 = {
    .x = 100,
    .y = 16,
    .sprite = &sign,
    .message = L"Hello, world!"
};

struct interactableObject sign2 = {
    .x = 50,
    .y = 16,
    .sprite = &sign,
    .message = L"I'm a evil sign... I'm going to over flow the text buffer and crash the game! Wait it's fixed?"
};

struct interactableObject sign3 = {
    .x = 70,
    .y = 16,
    .sprite = &sign,
    .message = L"I'm an outdoor sign! Those inside signs are so boring!"
};

struct interactableObject* houseObjects[] = {
    &sign1,
    &sign2,
};

const int numHouseObjects = sizeof(houseObjects) / sizeof(houseObjects[0]);

struct interactableObject* outsideObjects[] = {
    &sign3,
};

const int numOutsideObjects = sizeof(outsideObjects) / sizeof(outsideObjects[0]);

struct map {
    char map[8][10];
    struct interactableObject** objects;
    int numObjects;
};


struct map houseMap = {
    "WWWWWWWWWW",
    "WFFFFFFFFW",
    "WFFFFFFFFW",
    "WFFFFFFFFF",
    "WFFFFFFFFF",
    "WFFFFFFFFW",
    "WFFFFFFFFW",
    "WWWWWWWWWW",
    houseObjects,
    numHouseObjects,
};

struct map outsideMap = {
    "WEEEEEEEEE",
    "WGGGGGGGGE",
    "WGGGGGGGGE",
    "FGGGGGGGGE",
    "FGGGGGGGGE",
    "WGGGGGGGGE",
    "WGGGGGGGGE",
    "WEEEEEEEEE",
    outsideObjects,
    numOutsideObjects,
};