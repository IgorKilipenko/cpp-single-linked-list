#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
#include <vector>

using namespace std;

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next) : value(val), next_node(next) {}
        Type value;
        Node* next_node = nullptr;
    };

   public:
    template <typename ValueType>
    class BasicIterator;
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() : head_{}, size_{0} {}

    template <typename T>
    SingleLinkedList(T begin, T end) : SingleLinkedList() {
        for (auto ptr = begin; ptr != end; ++ptr) {
            PushFront(*ptr);
        }
    }

    SingleLinkedList(std::initializer_list<Type> values) : SingleLinkedList(std::rbegin(values), std::rend(values)) {}

    SingleLinkedList(const SingleLinkedList& other) : SingleLinkedList() {
        assert(size_ == 0 && head_.next_node == nullptr);

        *this = other;
    }

    ~SingleLinkedList() {
        Clear();
    }
    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        for (Node* node = head_.next_node; node != nullptr;) {
            Node* for_delete = node;
            node = node->next_node;
            delete for_delete;
        }
        head_.next_node = nullptr;
        size_ = 0;
    }

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{nullptr};
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    SingleLinkedList& operator=(const SingleLinkedList& other) {
        if (this == &other) {
            return *this;
        }
        if (other.size_ == 0) {
            Clear();
            return *this;
        }

        try {
            vector<Type> buffer(other.begin(), other.end());
            SingleLinkedList tmp{buffer.rbegin(), buffer.rend()};
            swap(tmp);
        } catch (...) {
            throw;
        }
        return *this;
    }

    bool operator==(const SingleLinkedList<Type>& other) const {
        if (IsEqualByRefs(other)) {
            return true;
        }

        return std::equal(this->begin(), this->end(), other.begin());
    }

    bool operator!=(const SingleLinkedList<Type>& other) const {
        return !(*this == other);
    }

    bool operator<(const SingleLinkedList<Type>& other) const {
        if (IsEqualByRefs(other)) {
            return false;
        }

        if (this->size_ == other.size_) {
            return !IsSuccessForAnyCrossItems(other, [](const Type& val, const Type& val_other) -> bool {
                return val >= val_other;
            });
        }

        return this->size_ < other.size_;
    }

    bool operator<=(const SingleLinkedList<Type>& other) const {
        if (IsEqualByRefs(other)) {
            return true;
        }

        if (this->size_ == other.size_) {
            return !IsSuccessForAnyCrossItems(other, [](const Type& val, const Type& val_other) -> bool {
                return val > val_other;
            });
        }

        return this->size_ < other.size_;
    }

    bool operator>(const SingleLinkedList<Type>& other) const {
        return !(*this <= other);
    }

    bool operator>=(const SingleLinkedList<Type>& other) const {
        return !(*this < other);
    }

    //! Тренажер требует наличия в интерфейсе класса функции с наименованием !swap
    /// Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr);

        auto new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator{new_node};
    }

    void PopFront() noexcept {
        assert(size_ > 0);

        auto front_node_it = head_.next_node;
        head_.next_node = front_node_it->next_node;

        delete front_node_it;
        --size_;
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert((pos.node_->next_node != nullptr));
        assert((size_ > 0));

        auto erased_node = pos.node_->next_node;
        pos.node_->next_node = erased_node->next_node;
        delete erased_node;
        --size_;
        return Iterator{pos.node_->next_node};
    }

   private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_;

    bool IsEqualByRefs(const SingleLinkedList& other) const {
        if (this->size_ != other.size_) {
            return false;
        }
        return this == &other || &this->head_ == &other.head_;
    }

    bool IsSuccessForAnyCrossItems(const SingleLinkedList& other, function<bool(const Type&, const Type&)> predicate) const {
        for (auto ptr = this->begin(), ptr_other = other.begin(); ptr != this->end() && ptr_other != other.end(); ++ptr, ++ptr_other) {
            if (predicate(*ptr, *ptr_other)) {
                return true;
            }
        }
        return false;
    }
};

// Шаблон класса «Базовый Итератор».
// Определяет поведение итератора на элементы односвязного списка
// ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
template <typename Type>
template <typename ValueType>
class SingleLinkedList<Type>::BasicIterator {
    // Класс списка объявляется дружественным, чтобы из методов списка
    // был доступ к приватной области итератора
    friend class SingleLinkedList;

   public:
    // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

    // Категория итератора — forward iterator
    // (итератор, который поддерживает операции инкремента и многократное разыменование)
    using iterator_category = std::forward_iterator_tag;
    // Тип элементов, по которым перемещается итератор
    using value_type = Type;
    // Тип, используемый для хранения смещения между итераторами
    using difference_type = std::ptrdiff_t;
    // Тип указателя на итерируемое значение
    using pointer = ValueType*;
    // Тип ссылки на итерируемое значение
    using reference = ValueType&;

    BasicIterator() : node_{nullptr} /*, before_begin_node_(nullptr) */ {}

    // Конвертирующий конструктор/конструктор копирования
    // При ValueType, совпадающем с Type, играет роль копирующего конструктора
    // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
    BasicIterator(const BasicIterator<Type>& other) noexcept : node_{other.node_} {}

    // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
    // пользовательского конструктора копирования, явно объявим оператор = и
    // попросим компилятор сгенерировать его за нас
    BasicIterator& operator=(const BasicIterator& rhs) = default;

    // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
    // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
    [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
        if ((node_ == nullptr && rhs.node_ == nullptr) || node_ == rhs.node_) {
            return true;
        }

        return false;
    }

    // Оператор проверки итераторов на неравенство
    // Противоположен ==
    [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
        return !(*this == rhs);
    }

    // Оператор сравнения итераторов (в роли второго аргумента итератор)
    // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
    [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
        return node_ == rhs.node_;
    }

    // Оператор проверки итераторов на неравенство
    // Противоположен ==
    [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
        return !(*this == rhs);
    }

    // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
    // Возвращает ссылку на самого себя
    // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
    BasicIterator& operator++() noexcept {
        assert(node_ != nullptr);
        node_ = node_->next_node;
        return *this;
    }

    // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
    // Возвращает прежнее значение итератора
    // Инкремент итератора, не указывающего на существующий элемент списка,
    // приводит к неопределённому поведению
    BasicIterator operator++(int) noexcept {
        auto old_value(*this);
        ++(*this);
        return old_value;
    }

    // Операция разыменования. Возвращает ссылку на текущий элемент
    // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
    // приводит к неопределённому поведению
    [[nodiscard]] reference operator*() const noexcept {
        assert(node_ != nullptr);
        return node_->value;
    }

    // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
    // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
    // приводит к неопределённому поведению
    [[nodiscard]] pointer operator->() const noexcept {
        assert(node_ != nullptr);
        return &(node_->value);
    }

   private:
    /// Конвертирующий конструктор итератора из указателя на узел списка
    explicit BasicIterator(Node* node) : node_(node) {}

    Node* node_ = nullptr;
};

// ----------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}
