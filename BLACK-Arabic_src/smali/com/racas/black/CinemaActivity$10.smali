.class Lcom/racas/black/CinemaActivity$10;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Landroid/media/MediaPlayer$OnPreparedListener;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/CinemaActivity;->startMovie(II)V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$0:Lcom/racas/black/CinemaActivity;

.field final synthetic val$start:I


# direct methods
.method constructor <init>(Lcom/racas/black/CinemaActivity;I)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010,
            0x1010
        }
        names = {
            null,
            null
        }
    .end annotation

    .annotation system Ldalvik/annotation/Signature;
        value = {
            "()V"
        }
    .end annotation

    .line 121
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    iput p2, p0, Lcom/racas/black/CinemaActivity$10;->val$start:I

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public onPrepared(Landroid/media/MediaPlayer;)V
    .locals 3

    .line 122
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$800(Lcom/racas/black/CinemaActivity;)Landroid/widget/SeekBar;

    move-result-object p1

    iget-object v0, p0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object v0

    invoke-virtual {v0}, Landroid/widget/VideoView;->getDuration()I

    move-result v0

    invoke-virtual {p1, v0}, Landroid/widget/SeekBar;->setMax(I)V

    iget-object p1, p0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object p1

    iget v0, p0, Lcom/racas/black/CinemaActivity$10;->val$start:I

    invoke-virtual {p1, v0}, Landroid/widget/VideoView;->seekTo(I)V

    iget-object p1, p0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object p1

    invoke-virtual {p1}, Landroid/widget/VideoView;->start()V

    .line 123
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$1000(Lcom/racas/black/CinemaActivity;)Landroid/os/Handler;

    move-result-object p1

    iget-object v0, p0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$900(Lcom/racas/black/CinemaActivity;)Ljava/lang/Runnable;

    move-result-object v0

    const-wide/16 v1, 0x1f4

    invoke-virtual {p1, v0, v1, v2}, Landroid/os/Handler;->postDelayed(Ljava/lang/Runnable;J)Z

    .line 124
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$10;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$1000(Lcom/racas/black/CinemaActivity;)Landroid/os/Handler;

    move-result-object p1

    new-instance v0, Lcom/racas/black/CinemaActivity$10$1;

    invoke-direct {v0, p0}, Lcom/racas/black/CinemaActivity$10$1;-><init>(Lcom/racas/black/CinemaActivity$10;)V

    const-wide/16 v1, 0xbb8

    invoke-virtual {p1, v0, v1, v2}, Landroid/os/Handler;->postDelayed(Ljava/lang/Runnable;J)Z

    .line 125
    return-void
.end method
