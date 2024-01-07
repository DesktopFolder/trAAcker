struct AppFocusImpl;
struct AppFocus
{
    AppFocusImpl* impl=nullptr;
    AppFocus() noexcept;
    ~AppFocus();
    void run();
};
