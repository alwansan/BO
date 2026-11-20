.class Lcom/racas/black/CinemaActivity$10$1;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Ljava/lang/Runnable;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/CinemaActivity$10;->onPrepared(Landroid/media/MediaPlayer;)V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$1:Lcom/racas/black/CinemaActivity$10;


# direct methods
.method constructor <init>(Lcom/racas/black/CinemaActivity$10;)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010
        }
        names = {
            null
        }
    .end annotation

    .line 124
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$10$1;->this$1:Lcom/racas/black/CinemaActivity$10;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public run()V
    .locals 2

    .line 124
    iget-object v0, p0, Lcom/racas/black/CinemaActivity$10$1;->this$1:Lcom/racas/black/CinemaActivity$10;

    iget-object v0, v0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$700(Lcom/racas/black/CinemaActivity;)Landroid/widget/LinearLayout;

    move-result-object v0

    if-eqz v0, :cond_0

    iget-object v0, p0, Lcom/racas/black/CinemaActivity$10$1;->this$1:Lcom/racas/black/CinemaActivity$10;

    iget-object v0, v0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$700(Lcom/racas/black/CinemaActivity;)Landroid/widget/LinearLayout;

    move-result-object v0

    const/16 v1, 0x8

    invoke-virtual {v0, v1}, Landroid/widget/LinearLayout;->setVisibility(I)V

    :cond_0
    return-void
.end method
