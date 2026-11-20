.class Lcom/racas/black/CinemaActivity$3;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Landroid/view/View$OnClickListener;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/CinemaActivity;->showList()V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$0:Lcom/racas/black/CinemaActivity;


# direct methods
.method constructor <init>(Lcom/racas/black/CinemaActivity;)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010
        }
        names = {
            null
        }
    .end annotation

    .line 84
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$3;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public onClick(Landroid/view/View;)V
    .locals 0

    .line 84
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$3;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-virtual {p1}, Lcom/racas/black/CinemaActivity;->finish()V

    return-void
.end method
