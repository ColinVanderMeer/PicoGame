struct interactableObject {
    int x, y;
    hagl_bitmap_t* sprite;
    int messageNumber;
    int numMessages;
    wchar_t* messages[];
};

struct interactableObject sign1 = {
    .x = 100,
    .y = 16,
    .sprite = &sign,
    .numMessages = 4,
    .messages = {
        L"Hello world!",
        L"Goodbye world!",
        L"The world exists!",
        L"stop talking to me",
    }
};

struct interactableObject sign2 = {
    .x = 50,
    .y = 16,
    .sprite = &sign,
    .numMessages = 2,
    .messages = {
        L"I'm a evil sign... I'm going to over flow the text buffer and crash the game!",
        L"Wait what do you mean it's been patched?",
    }
};

struct interactableObject sign3 = {
    .x = 70,
    .y = 16,
    .sprite = &sign,
    .numMessages = 2,
    .messages = {
        L"I'm an outdoor sign! Those inside signs are so boring!",
        L"(This is definitely the more boring sign)",
    }
};

struct interactableObject morayObj = {
    .x = 120,
    .y = 60,
    .sprite = &moray,
    .numMessages = 7,
    .messages = {
        L"What do you MEAN I'm in the wrong game!?!?",
        L"That's absurd!",
        L"Absolutely ridiculous!",
        L"Unbelievable!",
        L"Outrageous!",
        L"Preposterous!",
        L"I'm only here because someone can't draw sprites                            {They're right...}",
    }
};

struct interactableObject* houseObjects[] = {
    &sign1,
    &sign2,
};

const int numHouseObjects = sizeof(houseObjects) / sizeof(houseObjects[0]);

struct interactableObject* outsideObjects[] = {
    &sign3,
    &morayObj,
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