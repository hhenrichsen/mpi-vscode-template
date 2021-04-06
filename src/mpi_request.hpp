// template<typename T>
// class MPISendRequest {
//     static const DESTINATION_UNSET = -1;

//     int destination = DESTINATION_UNSET;
//     int count = 1;
//     int flag = 0;
//     T* data;

// public:
//     MPISendRequest to(int destination) {
//         this->destination = destination;
//         return *this;
//     }

//     MPISendRequest withFlag(int flag) {
//         this->flag = flag;
//         return *this;
//     }


// }

// class MPIRequest {
//     static MPISendRequest send() {

//     }
// }