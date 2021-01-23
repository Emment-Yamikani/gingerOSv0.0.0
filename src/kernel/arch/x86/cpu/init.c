extern void init_desc();
extern void install_handlers();

void init_cpu()
{
    init_desc();
    install_handlers();
}